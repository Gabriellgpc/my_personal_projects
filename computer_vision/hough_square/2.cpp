#include <string>
#include <opencv2/opencv.hpp>
#include <list>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;

#define _DEBUG_LVL1_

static constexpr uint16_t min_number_votes = 4;
static constexpr uint16_t max_number_votes = 1000;
static constexpr uint16_t max_square_size = 150;
static constexpr uint16_t min_square_size = 10; //menor lado considerado de um quadrado [px]
static constexpr uint16_t ang_resolution = 18;  //precisao de 5 graus
static constexpr long double mem_max = 2.0;     //[Gb]
static constexpr int number_max_detection = -1;
static constexpr float edge_threshold = 200;

static constexpr long double ang_step = M_PI_2 / ang_resolution;
static constexpr uint16_t step = 1; //passo de busca em (x,y). equivalente a redução na resolução da imagem

class MySquare
{
public:
    MySquare() : _xc(0), _yc(0), _size(0), _theta(0), _votes(0) {}
    MySquare(const uint16_t &xc,
             const uint16_t &yc,
             const double &theta,
             const uint16_t &size) : _xc(xc), _yc(yc), _size(size), _theta(theta), _votes(0) {}
    MySquare(const uint16_t &xc,
             const uint16_t &yc,
             const double &theta,
             const uint16_t &size,
             const uint16_t &votes) : _xc(xc), _yc(yc), _size(size), _theta(theta), _votes(votes) {}
    inline Point2f center() const { return Point2f((float)_xc, (float)_yc); }
    inline uint16_t &xc() { return _xc; }
    inline uint16_t &yc() { return _yc; }
    inline uint16_t &size() { return _size; }
    inline double &theta() { return _theta; }
    inline uint16_t isize() const { return (_size - min_square_size) % (max_square_size - min_square_size); }
    inline uint16_t itheta() //devolve o inteiro correspondente ao angulo no intervalo [0,90)
    {
        _theta = fabs(fmod(_theta, M_PI_2)); // mod pi/2. making sure that theta in [0,pi/2)
        return uint16_t(round(_theta * (ang_resolution / M_PI_2))) % ang_resolution;
    }
    inline uint16_t &votes() { return _votes; }

private:
    uint16_t _xc, _yc;
    uint16_t _size;
    double _theta; //angulo com relacao ao eixo horizontal
    uint16_t _votes;
};

//img: imagem com fundo branco e quadrados pretos
//max_detection: -1 para tentar achar todos, ou um número inteiro para limitar
std::list<MySquare> myHoughtTransform4Squares(const Mat &img, const int &max_detection = -1);
void drawSquares(const Mat &src, Mat &dest, const std::list<MySquare> &squares);
void convFilter(const Mat &src, Mat &dest, const Mat &mask);
void _drawSquare(const Mat &src, Mat &dest, const MySquare square);
void squares_supression(std::list<MySquare> &squares);

int main(int argc, char **argv)
{
    Mat img, output, output_raw;
    std::list<MySquare> squares;

    img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    if (!img.data)
    {
        std::cerr << "Erro ao carregar imagem\n";
        return -1;
    }
    std::cout << "Image Size = " << img.size() << '\n';
    /******************************************************/
    /*Verifica a quantidade de memoria que será necessaria*/
    /******************************************************/
    long double mem = (img.cols) * (img.rows) * (max_square_size - min_square_size) * ang_resolution * sizeof(uint16_t) * 1e-9; //[Gbyte]
    std::cout << "Memoria exigida na transformada de Hough:" << mem << " Gb\n";
    assert(mem <= mem_max);

    squares = myHoughtTransform4Squares(img, number_max_detection);
    drawSquares(img, output_raw, squares);

    std::cout << "Quadrados detectados, pre-supressao de quadrados:" << squares.size() << '\n';
    squares_supression(squares);
    std::cout << "Quadrados detectados, pos-supressao de quadrados:" << squares.size() << '\n';

    drawSquares(img, output, squares);
    //trecho apenas para exibir e salvar as imagens
    imshow("Entrada", img);
    imshow("Sem Supressão", output_raw);
    imshow("Saida", output);

    imwrite("entrada_2.png", img);
    imwrite("saida_bruta_2.png", output_raw);
    imwrite("saida_2.png", output);

    //aguarda uma tecla para encerrar o programa
    waitKey(0);
    return 0;
}

bool findNextNormalPoint(const Mat &img,
                         const Mat &gx,
                         const Mat &gy,
                         const Point2f &curr_normal,
                         const Point2f &curr_point,
                         Point2f &next_normal,
                         Point2f &next_point)
{
    static Point2f gv; //vetor gradiente atual
    double search_step = 2.0;
    double it = 1.0;
    static bool search_problem;

    next_normal = curr_normal;
    next_point = curr_point - curr_normal * (min_square_size / 2.0);
    do
    {
        it += search_step;
        next_point = curr_point - curr_normal * it;

        next_point = Point2i(next_point);
        
        //busca saiu dos limites da imagem
        search_problem = (next_point.x >= img.size().width) ||   //busca saiu dos limites da imagem
                         (next_point.y >= img.size().height)||
                         ((next_point.x < 0) || (next_point.y < 0))||
                         (norm(next_point - curr_point) > max_square_size); //caso tenha passado do tamanho maximo que um quadrado possa ter
        
        if(search_problem)
        {   
            it = 3.0;
            //maximo de reducao do passo de busca ou next_point nao esta andando
            if(search_step < 0.01)
                return false;
            search_step /= 2.0;
            continue;
        }
        
        gv = Point2f(gx.at<float>(next_point.y, next_point.x), gy.at<float>(next_point.y, next_point.x));
    } while (norm(gv) < edge_threshold);

    next_normal = gv / norm(gv);

    return true;
}

std::list<MySquare> myHoughtTransform4Squares(const Mat &img, const int &max_detection)
{
    // vector4D vote_table;
    //fazer com ponteiro pra não perder a pratica...
    //e pq é mt feio alocar o vetor4D com vector!
    uint16_t ****vote_table = NULL;

    uint16_t width = img.cols, height = img.rows;
    list<MySquare> sqs;
    Point2f normais[4];
    Point2f points[4];
    Mat gx, gy, gx_4save, gy_4save;
    Mat mask;
    //x crescente ->
    //y crescente \/
    float xSobel[] = {-1, 0, 1,
                      -2, 0, 2,
                      -1, 0, 1};
    float ySobel[] = {-1, -2, -1,
                      0, 0, 0,
                      1, 2, 1};

    /*************************************/
    /****** Obter o grande da imagem******/
    /*************************************/
    mask = Mat(3, 3, CV_32F, xSobel);
    convFilter(img, gx, mask);
    mask = Mat(3, 3, CV_32F, ySobel);
    convFilter(img, gy, mask);

    //apenas para exibir e salvar as imagens dos gradientes
    gx.convertTo(gx_4save, CV_8U, 255.0 / 2040.0, 127.0);
    gy.convertTo(gy_4save, CV_8U, 255.0 / 2040.0, 127.0);
    imshow("Gx", gx_4save);
    imshow("Gy", gy_4save);
    imwrite("gx_2.png",gx_4save);
    imwrite("gy_2.png",gy_4save);
    /*********************************************/
    /* Alocando memoria para a tabela de votação */
    /*********************************************/

    vote_table = new uint16_t ***[width];
    for (uint16_t x = 0; x < width; x++)
    {
        vote_table[x] = new uint16_t **[height];
        for (uint16_t y = 0; y < height; y++)
        {
            vote_table[x][y] = new uint16_t *[max_square_size - min_square_size];
            for (uint16_t l = 0; l < (max_square_size - min_square_size); l++)
            {
                vote_table[x][y][l] = new uint16_t[ang_resolution];
                memset(vote_table[x][y][l], 0, ang_resolution * sizeof(uint16_t));
            }
        }
    }
    /******************************************************/
    /*Percorrer a matriz de gradiente em busca das normais*/
    /******************************************************/

#ifdef _DEBUG_LVL1_
    uint16_t qtd_votos = 0;
#endif

    Point2f curr_gv; //vetor gradiente atual
    Point2f midle_p;
    MySquare sq;
    bool r;
    for (uint16_t y = 0; y < height; y += step)
        for (uint16_t x = 0; x < width; x += step)
        {
            curr_gv = Point2f(gx.at<float>(y, x), gy.at<float>(y, x));
            //caso gradiente fraco: ignorar
            if (norm(curr_gv) < edge_threshold)
                continue;

            sq.theta() = atan2(curr_gv.y, curr_gv.x) + M_PI; //atan2 in [-pi,pi) + M_PI => [0,2pi)

            points[0] = Point2i(x, y);
            normais[0] = curr_gv / norm(curr_gv);
            normais[0] = Point2f(round(normais[0].x), round(normais[0].y));

            //busca pela normal oposta
            r = findNextNormalPoint(img, gx, gy, normais[0], points[0], normais[1], points[1]);

            //caso de falha em achar a normal oposta: desista desse
            if (r == false)
                continue;

            sq.size() = (uint16_t)round(norm(points[0] - points[1]));
            if (sq.size() < min_square_size)
                continue;
            //buscar o terceiro ponto. esse ponto definida o centro do quadrado
            //rotacionar alguma das normais ja conhecida em 90 graus
            // |cos -sin| |x|
            // |sin  cos| |y|
            normais[2].x = normais[0].x * cos(M_PI_2) - normais[0].y * sin(M_PI_2);
            normais[2].y = normais[0].x * sin(M_PI_2) + normais[0].y * cos(M_PI_2);
            //ponto medio entre os pontos ja encontrados
            midle_p = (points[0] + points[1]) / 2.0;

            //utiliza essa normal e o ponto medio entre as bordas encontradas para encontrar o terceiro ponto
            r = findNextNormalPoint(img, gx, gy, normais[2], midle_p, normais[3], points[3]);

            if (r == false)
                continue;

            //com três pontos e a normal do terceiro ponto é possivel estimar o centro do quadrado
            midle_p = points[3] + normais[2] * (sq.size() / 2.0);

            sq.xc() = (uint16_t)round(midle_p.x) % width;
            sq.yc() = (uint16_t)round(midle_p.y) % height;

            vote_table[sq.xc()][sq.yc()][sq.isize()][sq.itheta()]++;
#ifdef _DEBUG_LVL1_
            std::cout << "Centro = " << sq.center() << '\n';
            std::cout << "Size   = " << sq.size() << " l = " << sq.isize() << '\n';
            std::cout << "Theta  = " << sq.itheta() * ang_step * 180 / M_PI << " a = " << sq.itheta() << '\n';
            std::cout << "Recebeu Voto!\n";
            std::cout << "Quantidade de votos do candidato = " << vote_table[sq.xc()][sq.yc()][sq.isize()][sq.itheta()] << '\n';
            qtd_votos++;
#endif
        }
#ifdef _DEBUG_LVL1_
    std::cout << "Quantidade de votos no total = " << (int)qtd_votos << '\n';
#endif
    /***************************************************/
    /*Hora de contar os votos e selecionar os quadrados*/
    /***************************************************/
    bool noStop = true;
    for (uint16_t y = 0; (y < height) && noStop; y++)
        for (uint16_t x = 0; (x < width) && noStop; x++)
            for (uint16_t l = 0; (l < max_square_size - min_square_size) && noStop; l++)
                for (uint16_t a = 0; (a < ang_resolution) && noStop; a++)
                {
                    if ((vote_table[x][y][l][a] >= min_number_votes) && (vote_table[x][y][l][a] <= max_number_votes))
                    {
#ifdef _DEBUG_LVL1_
                        std::cout << "Candidato com " << vote_table[x][y][l][a] << "votos" << '\n';
#endif

                        sqs.push_back(MySquare(x, y, a * ang_step, l + min_square_size, vote_table[x][y][l][a]));
                        if (max_detection < 0)
                            continue;
                        if (sqs.size() == (size_t)max_detection)
                            noStop = false;
                    }
                }

    /********************************************/
    /* Desalocando memoria da tabela de votação */
    /********************************************/
    for (uint16_t x = 0; x < width; x++)
    {
        for (uint16_t y = 0; y < height; y++)
        {
            for (uint16_t l = 0; l < (max_square_size - min_square_size); l++)
                delete[] vote_table[x][y][l];
            delete[] vote_table[x][y];
        }
        delete[] vote_table[x];
    }
    delete[] vote_table;

    return sqs;
}

void _drawSquare(const Mat &src, Mat &dest, MySquare square)
{
    RotatedRect rSquare = RotatedRect(square.center(), Size2f(square.size(), square.size()), square.theta() * 180.0 / M_PI);
    Point2f vertices[4];
    rSquare.points(vertices);
    for (uint8_t i = 0; i < 4; i++)
        line(dest, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255), 2);
}
void drawSquares(const Mat &src, Mat &dest, const std::list<MySquare> &squares)
{
    src.copyTo(dest);
    cvtColor(dest, dest, COLOR_GRAY2RGB);

    for (auto sq_it = squares.begin(); sq_it != squares.end(); sq_it++)
        _drawSquare(dest, dest, *sq_it);
}

void squares_supression(std::list<MySquare> &squares)
{
    std::vector<MySquare> sqs(squares.begin(), squares.end());
    std::list<MySquare> filtered_squares;
    std::vector<bool> toVisit(sqs.size(), true);
    MySquare tmpSquare;
    uint64_t xc_tmp, yc_tmp, size_tmp;
    double N, th_tmp;

    for (uint16_t sqA_it = 0; sqA_it < sqs.size(); sqA_it++)
    {
        if (toVisit[sqA_it] == false)
            continue;
        toVisit[sqA_it] = false;

        xc_tmp = sqs[sqA_it].xc();
        yc_tmp = sqs[sqA_it].yc();
        size_tmp = sqs[sqA_it].size();
        th_tmp = sqs[sqA_it].theta();

        N = 1;
        for (uint16_t sqB_it = sqA_it + 1; sqB_it < sqs.size(); sqB_it++)
        {
            if (toVisit[sqB_it] == false)
                continue;

            //verifica similaridade
            //caso a distancia entre os centros seja menor que o lado do quadrado: considerar iguais
            if (norm(sqs[sqA_it].center() - sqs[sqB_it].center()) <= min(sqs[sqA_it].size(), sqs[sqB_it].size()))
            {
                toVisit[sqB_it] = false;

                xc_tmp += sqs[sqB_it].xc();
                yc_tmp += sqs[sqB_it].yc();
                th_tmp += sqs[sqB_it].theta();
                size_tmp += sqs[sqB_it].size();

                N++;
            }
        }

        tmpSquare.xc() = xc_tmp / N;
        tmpSquare.yc() = yc_tmp / N;
        tmpSquare.theta() = th_tmp / N;
        tmpSquare.size() = size_tmp / N;

        filtered_squares.push_back(tmpSquare);
    }

    squares = filtered_squares;
}

void convFilter(const Mat &src, Mat &dest, const Mat &mask)
{
    float sum;
    Mat src_f;

    src.convertTo(src_f, CV_32F);
    dest = Mat::zeros(src.size(), CV_32F);

    for (int y = mask.rows / 2; y < (src.rows - mask.rows / 2); y++)
        for (int x = mask.cols / 2; x < (src.cols - mask.cols / 2); x++)
        {
            sum = 0;
            for (int v = 0; v < mask.rows; v++)
                for (int u = 0; u < mask.cols; u++)
                {
                    sum += src_f.at<float>(y - mask.rows / 2 + v, x - mask.cols / 2 + u) * mask.at<float>(v, u);
                }
            dest.at<float>(y, x) = sum;
        }
}
