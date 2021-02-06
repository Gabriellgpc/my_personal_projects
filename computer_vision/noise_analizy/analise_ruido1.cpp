#include <opencv2/opencv.hpp>
#include <string>
#include <cmath>

#define PATH_OUT "saida_analise1"

using namespace cv;
using namespace std;

#define I_MAX 255

float calcPSNR(const Mat &ref, const Mat &img);
void calcHistograma(const Mat &src, uint32_t histograma[]);
//Esta função cria uma imagem, contendo o plot do histograma
//com a dimensão de 256x200 px
void drawHistograma(const uint32_t histograma[], Mat &imgH);

int main(int argc, char** argv){
  Mat orig, imgH;
  String path("./primeiroConjunto/*.jpg");
  std::vector<String> fn;
  std::vector<Mat> imagens;
  uint32_t histograma[I_MAX+1];

  glob(path, fn, true);
  orig = imread(fn[0], CV_LOAD_IMAGE_GRAYSCALE);
  if(!orig.data)
  {
    std::cerr << "Erro ao carregar a imagem original" << '\n';
    return-1;
  }

  // carrega as N imagens
  for(size_t i = 0; i < fn.size(); i++)
  {
    Mat im = imread(fn[i], CV_LOAD_IMAGE_GRAYSCALE);
    if(!im.data)continue;//caso falhe na leitura, vai para o proximo arquivo
    imagens.push_back(im);

    calcHistograma(im, histograma);
    drawHistograma(histograma, imgH);
    imshow("imagem", im);
    imshow("Histograma", imgH);
    imwrite(string(PATH_OUT/"imagem") + to_string(i) + string(".png"), im);
    imwrite(string(PATH_OUT/"histograma") + to_string(i) + string(".png"), imgH);

    std::cout << "PSNR:"<< calcPSNR(imagens[0], im) << '\n';

    waitKey(0);
  }

  if(imagens.empty())
  {
    std::cerr << "Erro ao carrega as imagens de " << path << "\n";
    return -1;
  }

  return 0;
}

void calcHistograma(const Mat &src, uint32_t histograma[])
{
  memset(histograma, 0, (I_MAX+1)*sizeof(uint32_t) ); //zera todas as posições do vetor de freq.

  for(int y = 0; y < src.rows; y++)
    for(int x = 0; x < src.cols; x++)
      histograma[src.at<uint8_t>(y,x)]++;
}

uint32_t maxValue(const uint32_t v[], size_t size)
{
  uint32_t max = 0;
  for(size_t i = 0; i < size; i++)
    max = (v[i] > max)?v[i]:max;
  return max;
}

void drawHistograma(const uint32_t histograma[], Mat &imgH)
{
  imgH = Mat::zeros(200, 256, CV_8U);
  uint32_t freqMax = maxValue(histograma, I_MAX+1);

  for(uint32_t i = 0; i < I_MAX+1; i++)
  {
    line(imgH, Point(i, 200), Point(i, 200-histograma[i]*(200.0/freqMax)), 255);
  }
}

float calcPSNR(const Mat &ref, const Mat &img)
{
  float mse = 0;
  for(int y = 0; y < ref.rows; y++)
  for(int x = 0; x < ref.cols; x++)
  {
    mse += pow(img.at<uint8_t>(y,x) - ref.at<uint8_t>(y,x), 2.0);
  }
  mse = mse/(ref.cols*ref.rows);
  return 10.0*log10((float)(I_MAX*I_MAX)/mse);
}
