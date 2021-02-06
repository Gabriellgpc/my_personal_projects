// O programa recebe uma imagem, transformada para escala de cinzas e aplica
// uma correção gamma e desenha listas verticais pretas de forma a tentar aplicar um efeito
// visual que dificulta a visualização da imagem original, porém é possivel visualizar se borrar
// a imagem de saída (tentar visualizar com a vista borrada já serve)
#include <cmath>
#include <opencv2/opencv.hpp>
using namespace cv;

#define LINE_WIDTH 5//px

void gammaCorrection(const Mat &img, Mat &out,double gamma)
{
  static Mat lookupTable(1, 256, CV_8U);
  static bool first = true;

  out = img.clone();

  for(int i = 0; i < 256 && first; i++)
    lookupTable.ptr()[i] = saturate_cast<uint8_t>(pow(i/255.0, gamma)*255);
  first = false;

  cv::LUT(img, lookupTable, out);
}

int main(int argc, char** argv){
  Mat img;
  Mat result;

  img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  if(!img.data)
  {
    std::cerr << "Erro ao carregar imagem!\n";
    return -1;
  }

  gammaCorrection(img, result, 0.05);

  imshow("Saida", img);
  waitKey(0);

  imshow("Saida", result);
  waitKey(0);
  
  //pinta faixas pretas com largura de LINE_WIDTH pixels
  //espaçadas de LINE_WIDTH
  for(int y = 0; y < img.rows; y++)
  for(int x = 0; x < img.cols; x++)
  {
    result.at<uint8_t>(y,x) = 0;
    if((x+1)%LINE_WIDTH == 0)x += LINE_WIDTH;
  }

  imshow("Saida", result);
  waitKey(0);

  imwrite("entradaEfeito.png", img);
  imwrite("resultadoEfeito.png", result);
  return 0;
}
