#include <opencv2/opencv.hpp> //all in opencv2
#include <string> //string, memset, memcpy
#include <cmath>  //sqrt, M_PI, pow
#include <list>   //list
#include <stdio.h> //puts

using namespace cv;
using namespace std;

#define I_MAX 255
#define PATH_OUT "saida_analise2"

float calcRMSE(const Mat &ref, const Mat &img);
void  calcHistograma(const Mat &src, uint32_t histograma[]);
void  drawHistograma(const uint32_t histograma[], Mat &imgH);
void  paintScore(Mat &img, uint32_t score);

/**** Filtros ****/
//Filtros com 2 parametros: m,n
void filtro_media(const Mat src, Mat &dest, uint32_t m, uint32_t n);
void filtro_media_geometrica(const Mat src, Mat &dest, uint32_t m, uint32_t n);
void filtro_mediana(const Mat src, Mat &dest, uint32_t m, uint32_t n);
void filtro_max(const Mat src, Mat &dest, uint32_t m, uint32_t n);
void filtro_min(const Mat src, Mat &dest, uint32_t m, uint32_t n);
void filtro_ponto_medio(const Mat src, Mat &dest, uint32_t m, uint32_t n);
void filtro_media_harmonica(const Mat src, Mat &dest, uint32_t m, uint32_t n);
//Filtros com 3 parametros: m,n, x
//para q: ordem do filtro
void filtro_media_contra_harmonica(const Mat src, Mat &dest, uint32_t m, uint32_t n, uint32_t q);
void filtro_media_alpha_cortada(const Mat src, Mat &dest, uint32_t m, uint32_t n, uint32_t d);


int main(int argc, char** argv)
{
  Mat orig;
  Mat imgH;
  Mat dest;
  String path("./segundoConjunto/*.jpg");
  std::vector<String> fn;
  std::vector<Mat> imagens;
  uint32_t histograma[I_MAX+1];

  glob(path, fn, true);
  // carrega as N imagens
  for(size_t i = 0; i < fn.size(); i++)
  {
    Mat im = imread(fn[i], CV_LOAD_IMAGE_GRAYSCALE);
    if(!im.data)continue;//caso falhe na leitura, vai para o proximo arquivo
    imagens.push_back(im);

    std::cout << "******** Imagem:"<< i << "********\n";
    // calcHistograma(im, histograma);
    // drawHistograma(histograma, imgH);
    imshow("Imagem", im);
    // imshow("Histograma", imgH);
    // imwrite(string("imagem") + to_string(i) + string(".png"), im);
    // imwrite(string("histograma") + to_string(i) + string(".png"), imgH);
    std::cout << "RMSE:"<< calcRMSE(imagens[0], im) << '\n';

    filtro_media(im, dest, 3, 3);
    std::cout << "filtro_media RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
    waitKey(0);

    filtro_media_geometrica(im, dest, 10, 10);
    std::cout << "filtro_media_geometrica RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
    waitKey(0);

    filtro_mediana(im, dest, 3, 3);
    std::cout << "filtro_mediana RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
    waitKey(0);
    //
    filtro_max(im, dest, 3, 3);
    std::cout << "filtro_max RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
    waitKey(0);
    //
    filtro_min(im, dest, 3, 3);
    std::cout << "filtro_min RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
    waitKey(0);
    //
    filtro_ponto_medio(im, dest, 3, 3);
    std::cout << "filtro_ponto_medio RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
    waitKey(0);
    //
    filtro_media_harmonica(im, dest, 10, 10);
    std::cout << "filtro_media_harmonica RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
    waitKey(0);
    //
    filtro_media_contra_harmonica(im, dest, 3, 3, 2);
    std::cout << "filtro_media_contra_harmonica RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
    waitKey(0);
    //
    filtro_media_alpha_cortada(im, dest, 3, 3, 2);
    std::cout << "filtro_media_alpha_cortada RMSE:"<< calcRMSE(imagens[0], dest) << '\n';
    imshow("Filtrada", dest);
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

uint32_t _maxValue(const uint32_t v[], size_t size)
{
  uint32_t max = 0;
  for(size_t i = 0; i < size; i++)
    max = (v[i] > max)?v[i]:max;
  return max;
}

void drawHistograma(const uint32_t histograma[], Mat &imgH)
{
  imgH = Mat::zeros(200, 256, CV_8U);
  uint32_t freqMax = _maxValue(histograma, I_MAX+1);

  for(uint32_t i = 0; i < I_MAX+1; i++)
  {
    line(imgH, Point(i, 200), Point(i, 200-histograma[i]*(200.0/freqMax)), 255);
  }
}

float calcRMSE(const Mat &ref, const Mat &img)
{
  float mse = 0;
  for(int y = 0; y < ref.rows; y++)
  for(int x = 0; x < ref.cols; x++)
  {
    mse += pow(img.at<uint8_t>(y,x) - ref.at<uint8_t>(y,x), 2.0);
  }
  mse = mse/(ref.cols*ref.rows);
	return sqrt(mse);
}

void filtro_media(const Mat src, Mat &dest, uint32_t m, uint32_t n)
{
  uint32_t sum = 0;
  dest = Mat::zeros(src.size(), CV_8U);

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    sum = 0;
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
        sum+= src.at<uint8_t>(t,s);
    dest.at<uint8_t>(y,x) = sum/(m*n);
  }
}
void filtro_media_harmonica(const Mat src, Mat &dest, uint32_t m, uint32_t n)
{
  double sum = 0;
  dest = Mat::zeros(src.size(), CV_8U);

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    sum = 0.0;
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
        sum+= 1.0/(src.at<uint8_t>(t,s)+1.0);  //somo 1 para evitar div por zero
    dest.at<uint8_t>(y,x) = (m*n)/sum;
  }
}
void filtro_media_geometrica(const Mat src, Mat &dest, uint32_t m, uint32_t n)
{
  double mult = 1;
  dest = Mat::zeros(src.size(), CV_8U);

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    mult = 1.0;
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
        mult*= (double)src.at<uint8_t>(t,s);
    dest.at<uint8_t>(y,x) = (uint8_t)pow(mult, 1.0/(m*n));
  }
}
void filtro_mediana(const Mat src, Mat &dest, uint32_t m, uint32_t n)
{
  list<uint8_t> lista;
  list<uint8_t>::iterator it;

  dest = Mat::zeros(src.size(), CV_8U);

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    lista.clear();
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
        lista.push_back(src.at<uint8_t>(t,s));
    lista.sort();
    it = lista.begin();
    for(uint32_t i = 0; i < lista.size()/2; i++)it++; //posiciona o iterator no meio do vetor
    if( lista.size()%2 == 0) //caso size par
      dest.at<uint8_t>(y,x) = (*it + *(++it))/2;
    else //caso size impar
      dest.at<uint8_t>(y,x) = *it;
  }
}
void filtro_max(const Mat src, Mat &dest, uint32_t m, uint32_t n)
{
  list<uint8_t> lista;

  dest = Mat::zeros(src.size(), CV_8U);

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    lista.clear();
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
        lista.push_back(src.at<uint8_t>(t,s));
    lista.sort();
    dest.at<uint8_t>(y,x) = *(--lista.end());
  }

}
void filtro_min(const Mat src, Mat &dest, uint32_t m, uint32_t n)
{
  list<uint8_t> lista;
  dest = Mat::zeros(src.size(), CV_8U);

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    lista.clear();
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
        lista.push_back(src.at<uint8_t>(t,s));
    lista.sort();
    dest.at<uint8_t>(y,x) = *lista.begin();
  }

}
void filtro_ponto_medio(const Mat src, Mat &dest, uint32_t m, uint32_t n)
{
  list<uint8_t> lista;
  dest = Mat::zeros(src.size(), CV_8U);

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    lista.clear();
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
        lista.push_back(src.at<uint8_t>(t,s));
    lista.sort();
    dest.at<uint8_t>(y,x) = (*lista.begin() + *(--lista.end()))/2;
  }
}
void filtro_media_contra_harmonica(const Mat src, Mat &dest, uint32_t m, uint32_t n, uint32_t q)
{
  double numerador   = 0.0;
  double denominador = 0.0;

  dest = Mat::zeros(src.size(), CV_8U);

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    numerador   = 0.0;
    denominador = 0.0;
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
      {
        numerador   += pow((double)src.at<uint8_t>(t,s),  (double)q + 1.0);
        denominador += pow((double)src.at<uint8_t>(t,s),  (double)q);
      }
    dest.at<uint8_t>(y,x) = (uint8_t)(numerador/denominador);
  }
}
void filtro_media_alpha_cortada(const Mat src, Mat &dest, uint32_t m, uint32_t n, uint32_t d)
{
  list<uint8_t> lista;
  list<uint8_t>::iterator it;
  uint32_t sum = 0;
  uint32_t i = 0;

  dest = Mat::zeros(src.size(), CV_8U);
  if(d >= m*n)
  {
    puts("Parametro invalido em filtro_media_alpha_cortada");
    return;
  }

  for(uint32_t y = n/2; y < (src.rows - n/2); y++)
  for(uint32_t x = m/2; x < (src.cols - m/2); x++)
  {
    lista.clear();
    for(uint32_t t = (y - n/2); t < (y + n/2); t++)
      for(uint32_t s = (x - m/2); s < (x+m/2); s++)
        lista.push_back(src.at<uint8_t>(t,s));
    lista.sort();

    i   = 0;
    sum = 0;
    for(it = lista.begin(); it != lista.end(); it++)
    {
      if((i >= d/2) && i <= (lista.size() - d))
        sum+= *it;
      i++;
    }
    dest.at<uint8_t>(y,x) = sum/(m*n - d);
  }
}
