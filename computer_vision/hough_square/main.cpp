#include <string>
#include <opencv2/opencv.hpp>
#include <list>
#include <cmath>
#include <vector>
#include "myUtils.hpp"

using namespace cv;
using namespace std;
using namespace myUtils;

static constexpr uint16_t min_number_votes = 4;
static constexpr uint16_t max_number_votes = 1000;
static constexpr uint16_t max_square_size = 150;
static constexpr uint16_t min_square_size = 10; //menor lado considerado de um quadrado [px]
static constexpr uint16_t ang_resolution = 18;  //precisão de 5 graus
static constexpr long double mem_max = 2.0;     //[Gb]
static constexpr int number_max_detection = -1;
static constexpr float edge_threshold = 200;

// static constexpr long double ang_step = M_PI_2 / ang_resolution;
static constexpr uint16_t step = 1; //passo de busca em (x,y). equivalente a redução na resolução da imagem

int main(int argc, char **argv)
{
    Mat img, output, output_raw;
    std::list<MySquare> squares;

    img = imread(argv[1], IMREAD_GRAYSCALE);
    if (!img.data)
    {
        std::cerr << "Erro ao carregar imagem\n";
        return -1;
    }
    std::cout << "Image Size = " << img.size() << '\n';
    /******************************************************/
    /*Verifica a quantidade de memoria que será necessária*/
    /******************************************************/
    long double mem = (img.cols) * (img.rows) * (max_square_size - min_square_size) * ang_resolution * sizeof(uint16_t) * 1e-9; //[Gbyte]
    std::cout << "Memoria exigida na transformada de Hough:" << mem << " Gb\n";
    assert(mem <= mem_max);

    squares = myHoughtTransform4Squares(img, number_max_detection,
                                        min_square_size,
                                        max_square_size,
                                        ang_resolution,
                                        edge_threshold,
                                        min_number_votes,
                                        max_number_votes);
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