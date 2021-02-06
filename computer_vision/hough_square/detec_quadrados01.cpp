#include <string>
#include <opencv2/opencv.hpp>
#include <list>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;

struct circle_t
{
  double xc, yc;
  double radius;
};

void convFilter(const Mat &src, Mat &dest, const Mat &mask);
void hough4circles(const Mat &img, const Mat&edge, list<circle_t> &circles, const uint32_t &max_circles,const uint &inf, const uint &sup);
void drawCircles(Mat &img, const list<circle_t> &circles);
bool testCircleIsOutside(const Mat &img, const circle_t &c, const uint8_t &background);

int main(int argc, char **argv)
{
  Mat img, edge_img, result, gx, gy;
  Mat mask;
  float xSobel[] = {-1, 0, 1,
                    -2, 0, 2,
                    1, 0, 1};
  float ySobel[] = {1, 2, 1,
                    0, 0, 0,
                    -1, -2, -1};

  img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  if (!img.data)
  {
    std::cerr << "Erro ao carregar imagem\n";
    return -1;
  }

  Canny(img, edge_img, 0, 100);
  mask = Mat(3, 3, CV_32F, xSobel);
  convFilter(img, gx, mask);
  mask = Mat(3, 3, CV_32F, ySobel);
  convFilter(img, gy, mask);

  //desenhando circulos
  list<circle_t> circles;
  
  std::cout << "Transformada de Hough...\n";
  hough4circles(img, edge_img, circles, 1, 2, 100000);
  std::cout << "Quantidade de circulos identificados:" << circles.size() << '\n';
  edge_img.copyTo(result);
  drawCircles(result, circles);

  //trecho apenas para exibir e salvar as imagens
  imshow("Entrada", img);
  imshow("Imagem de bordas", edge_img);
  imshow("Resultado", result);

  // gx.convertTo(gx, CV_8U, 255.0/2040.0, 127.0);
  // imshow("Gx", gx);
  // gy.convertTo(gy, CV_8U, 255.0/2040.0, 127.0);
  // imshow("Gy", gy);

  waitKey(0);
  return 0;
}

bool testCircleIsOutside(const Mat &img, const circle_t &c, const uint8_t &background)
{
  uint64_t x,y;
  double th;
  const double step_theta = 2.0 * M_PI / 100.0;
  uint32_t count = 0, threshold = 2;


  for(th = 0; th < 2.0*M_PI; th+= step_theta)
  {
    x = static_cast<uint64_t>(round(c.xc + c.radius*cos(th)));
    y = static_cast<uint64_t>(round(c.yc + c.radius*sin(th)));
    if((x < img.cols) && (y < img.rows))
      if(img.at<uint8_t>(y,x) == background)count++;
    else
      count++;
    if(count > threshold)
      return true;
  }

  return false;
}

void hough4circles(const Mat &img, const Mat&edge, list<circle_t> &circles, const uint32_t &max_circles, const uint &inf, const uint &sup)
{
  uint32_t H = img.size().height;
  uint32_t W = img.size().width;
  const uint32_t min_radius = 1, max_radius = sqrt( W * W + H * H);
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
      if(edge.at<uint8_t>(row,col) == 0)continue;
      //varre os diferentes raios
      for (uint32_t r = min_radius; r < max_radius; r++)
        for (float th = 0; th < 2.0 * M_PI && not_stop ; th += step_theta) //varre de [0 a 2pi[
        {
          x = static_cast<uint32_t>(round(col + r * cos(th)));
          y = static_cast<uint32_t>(round(row + r * sin(th)));
          if ((x < W) && (y < H))
          {
            //testa se centro do circulo esta numa regiao de objeto
            if(img.at<uint8_t>(y,x) == 255) continue;
            
            c.xc = x;
            c.yc = y;
            c.radius = r + min_radius;
            //testa se algum ponto da circunferencia esta fora do objeto
            if(testCircleIsOutside(img, c, 255))continue;
            
            coefMat[x][y][r - min_radius]++;
            if ((coefMat[x][y][r] >= inf) && (coefMat[x][y][r] <= sup))
            {
              circles.push_back(c);
              if(circles.size() == max_circles)
                not_stop = false;
            }
          }
        }
    }

  for (x = 0; x < W; x++)
    for (y = 0; y < H; y++)
      delete[] coefMat[x][y];
}

void drawCircles(Mat &img, const list<circle_t> &circles)
{
  Point p;
  for (auto it = circles.begin(); it != circles.end(); it++)
  {
    p.x = it->xc;
    p.y = it->yc;
    circle(img, p, it->radius, 255, 1, 8);
  }
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
