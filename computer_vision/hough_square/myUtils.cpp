#include "myUtils.hpp"

// using namespace cv;
using namespace myUtils;
using namespace std;

uint16_t myUtils::MySquare::isize(const uint16_t &min_square_size,
                                  const uint16_t &max_square_size) const
{
  return (_size - min_square_size) % (max_square_size - min_square_size);
}

uint16_t myUtils::MySquare::itheta(const uint16_t &ang_resolution)
{
  _theta = fabs(fmod(_theta, M_PI_2)); // mod pi/2. making sure that theta in [0,pi/2)
  return uint16_t(round(_theta * (ang_resolution / M_PI_2))) % ang_resolution;
}

void myUtils::hough4circles(const cv::Mat &img,
                            const cv::Mat &edge,
                            list<circle_t> &circles,
                            const uint32_t &max_circles,
                            const uint &inf, const uint &sup)
{
  uint32_t H = img.size().height;
  uint32_t W = img.size().width;
  const uint32_t min_radius = 1, max_radius = sqrt(W * W + H * H);
  const uint32_t n_radius = max_radius - min_radius; //numero de raios considerados
  const double step_theta = 2.0 * M_PI / 200.0;
  uint32_t *coefMat[W][H];
  uint32_t x;
  uint32_t y;
  bool not_stop = true;
  circle_t c;

  for (x = 0; x < W; x++)
    for (y = 0; y < H; y++)
    {
      coefMat[x][y] = new uint32_t[n_radius];
      memset(coefMat[x][y], 0, n_radius); //inicializa a tabela com zero
    }

  for (uint row = 0; row < H; row++)
    for (uint col = 0; col < W; col++)
    {
      //ignora background preto
      if (edge.at<uint8_t>(row, col) == 0)
        continue;
      //varre os diferentes raios
      for (uint32_t r = min_radius; r < max_radius; r++)
        for (float th = 0; th < 2.0 * M_PI && not_stop; th += step_theta) //varre de [0 a 2pi[
        {
          x = static_cast<uint32_t>(round(col + r * cos(th)));
          y = static_cast<uint32_t>(round(row + r * sin(th)));
          if ((x < W) && (y < H))
          {
            //testa se centro do circulo esta numa região de objeto
            if (img.at<uint8_t>(y, x) == 255)
              continue;

            c.xc = x;
            c.yc = y;
            c.radius = r + min_radius;
            //testa se algum ponto da circunferência esta fora do objeto
            if (testCircleIsOutside(img, c, 255))
              continue;

            coefMat[x][y][r - min_radius]++;
            if ((coefMat[x][y][r] >= inf) && (coefMat[x][y][r] <= sup))
            {
              circles.push_back(c);
              if (circles.size() == max_circles)
                not_stop = false;
            }
          }
        }
    }

  for (x = 0; x < W; x++)
    for (y = 0; y < H; y++)
      delete[] coefMat[x][y];
}

void myUtils::drawCircles(cv::Mat &img, const list<circle_t> &circles)
{
  cv::Point p;
  for (auto it = circles.begin(); it != circles.end(); it++)
  {
    p.x = it->xc;
    p.y = it->yc;
    circle(img, p, it->radius, 255, 1, 8);
  }
}

bool findNextNormalPoint(const cv::Mat &img,
                         const cv::Mat &gx,
                         const cv::Mat &gy,
                         const cv::Point2f &curr_normal,
                         const cv::Point2f &curr_point,
                         cv::Point2f &next_normal,
                         cv::Point2f &next_point,
                         const uint16_t &min_square_size,
                         const uint16_t &max_square_size,
                         const float &edge_threshold)
{
  static cv::Point2f gv; //vetor gradiente atual
  double search_step = 2.0;
  double it = 1.0;
  static bool search_problem;

  next_normal = curr_normal;
  next_point = curr_point - curr_normal * (min_square_size / 2.0);
  do
  {
    it += search_step;
    next_point = curr_point - curr_normal * it;

    next_point = cv::Point2i(next_point);

    //busca saiu dos limites da imagem
    search_problem = (next_point.x >= img.size().width) || //busca saiu dos limites da imagem
                     (next_point.y >= img.size().height) ||
                     ((next_point.x < 0) || (next_point.y < 0)) ||
                     (norm(next_point - curr_point) > max_square_size); //caso tenha passado do tamanho maximo que um quadrado possa ter

    if (search_problem)
    {
      it = 3.0;
      //maximo de reducao do passo de busca ou next_point nao esta andando
      if (search_step < 0.01)
        return false;
      search_step /= 2.0;
      continue;
    }

    gv = cv::Point2f(gx.at<float>(next_point.y, next_point.x), gy.at<float>(next_point.y, next_point.x));
  } while (norm(gv) < edge_threshold);

  next_normal = gv / norm(gv);

  return true;
}

std::list<MySquare>
myUtils::myHoughtTransform4Squares(const cv::Mat &img,
                                   const int &max_detection,
                                   const uint16_t &min_square_size,
                                   const uint16_t &max_square_size,
                                   const uint16_t &ang_resolution,
                                   const float &edge_threshold,
                                   const uint16_t &min_number_votes,
                                   const uint16_t &max_number_votes)
{
  const long double ang_step = M_PI_2 / ang_resolution;
  // vector4D vote_table;
  //fazer com ponteiro pra não perder a pratica...
  //e pq é mt feio alocar o vetor4D com vector!
  uint16_t ****vote_table = NULL;

  uint16_t width = img.cols, height = img.rows;
  list<MySquare> sqs;
  cv::Point2f normais[4];
  cv::Point2f points[4];
  cv::Mat gx, gy, gx_4save, gy_4save;
  cv::Mat mask;
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
  mask = cv::Mat(3, 3, CV_32F, xSobel);
  convFilter(img, gx, mask);
  mask = cv::Mat(3, 3, CV_32F, ySobel);
  convFilter(img, gy, mask);

  //apenas para exibir e salvar as imagens dos gradientes
  gx.convertTo(gx_4save, CV_8U, 255.0 / 2040.0, 127.0);
  gy.convertTo(gy_4save, CV_8U, 255.0 / 2040.0, 127.0);
  imshow("Gx", gx_4save);
  imshow("Gy", gy_4save);
  imwrite("gx_2.png", gx_4save);
  imwrite("gy_2.png", gy_4save);
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

  cv::Point2f curr_gv; //vetor gradiente atual
  cv::Point2f midle_p;
  MySquare sq;
  bool r;
  for (uint16_t y = 0; y < height; y += 1)
    for (uint16_t x = 0; x < width; x += 1)
    {
      curr_gv = cv::Point2f(gx.at<float>(y, x), gy.at<float>(y, x));
      //caso gradiente fraco: ignorar
      if (norm(curr_gv) < edge_threshold)
        continue;

      sq.theta() = atan2(curr_gv.y, curr_gv.x) + M_PI; //atan2 in [-pi,pi) + M_PI => [0,2pi)

      points[0] = cv::Point2i(x, y);
      normais[0] = curr_gv / norm(curr_gv);
      normais[0] = cv::Point2f(round(normais[0].x), round(normais[0].y));

      //busca pela normal oposta
      r = findNextNormalPoint(img, gx, gy,
                              normais[0],
                              points[0], normais[1], points[1],
                              min_square_size, max_square_size, edge_threshold);

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
      //ponto médio entre os pontos ja encontrados
      midle_p = (points[0] + points[1]) / 2.0;

      //utiliza essa normal e o ponto médio entre as bordas encontradas para encontrar o terceiro ponto
      r = findNextNormalPoint(img, gx, gy, normais[2], midle_p, normais[3], points[3],
                              min_square_size, max_square_size, edge_threshold);

      if (r == false)
        continue;

      //com três pontos e a normal do terceiro ponto é possível estimar o centro do quadrado
      midle_p = points[3] + normais[2] * (sq.size() / 2.0);

      sq.xc() = (uint16_t)round(midle_p.x) % width;
      sq.yc() = (uint16_t)round(midle_p.y) % height;

      vote_table[sq.xc()][sq.yc()][sq.isize(min_square_size, max_square_size)][sq.itheta(ang_resolution)]++;
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

void myUtils::_drawSquare(const cv::Mat &src, cv::Mat &dest, MySquare square)
{
  cv::RotatedRect rSquare = cv::RotatedRect(square.center(), cv::Size2f(square.size(), square.size()), square.theta() * 180.0 / M_PI);
  cv::Point2f vertices[4];
  rSquare.points(vertices);
  for (uint8_t i = 0; i < 4; i++)
    line(dest, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 0, 255), 2);
}
void myUtils::drawSquares(const cv::Mat &src, cv::Mat &dest, const std::list<MySquare> &squares)
{
  src.copyTo(dest);
  cvtColor(dest, dest, cv::COLOR_GRAY2RGB);

  for (auto sq_it = squares.begin(); sq_it != squares.end(); sq_it++)
    _drawSquare(dest, dest, *sq_it);
}

void myUtils::squares_supression(std::list<MySquare> &squares)
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

void myUtils::convFilter(const cv::Mat &src, cv::Mat &dest, const cv::Mat &mask)
{
  float sum;
  cv::Mat src_f;

  src.convertTo(src_f, CV_32F);
  dest = cv::Mat::zeros(src.size(), CV_32F);

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

bool myUtils::testCircleIsOutside(const cv::Mat &img, const circle_t &c, const uint8_t &background)
{
  int x, y;
  double th;
  const double step_theta = 2.0 * M_PI / 100.0;
  uint32_t count = 0, threshold = 2;

  for (th = 0; th < 2.0 * M_PI; th += step_theta)
  {
    x = static_cast<int>(round(c.xc + c.radius * cos(th)));
    y = static_cast<int>(round(c.yc + c.radius * sin(th)));
    if ((x < img.cols) && (y < img.rows))
    {
      if (img.at<uint8_t>(y, x) == background)
        count++;
    }
    if (count > threshold)
      return true;
  }

  return false;
}