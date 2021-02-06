// O programa recebe uma imagem e um gradiente na vertical.
// O exibe e salva a imagem do gradiente e a imagem da imagem com o efeito aplicado.
#include <opencv2/opencv.hpp>
using namespace cv;

int main(int argc, char**argv)
{
  Mat img;
  Mat result;
  Mat gradient;

  img = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);

  if(!img.data){
    std::cout << "imagem nao carregou corretamente\n";
    return(-1);
  }
  imshow("Original", img);

  result   = Mat(img.size(), CV_32FC1);
  gradient = img.clone();

  for(int y = 0; y < img.rows; y++)
  for(int x = 0; x < img.cols; x++)
  {
    result.at<float>(y,x) = (img.at<uint8_t>(y,x)/255.0)*y*(1.0/img.rows);
    gradient.at<uint8_t>(y,x) = y*(255.0/img.rows);
  }
  //converte para um formato que eh possivel salvar, neste caso equivalente ao grayscale
  //https://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#void%20Mat::convertTo(OutputArray%20m,%20int%20rtype,%20double%20alpha,%20double%20beta)%20const
  result.convertTo(result, CV_8UC1, 255);

  imshow("Resultado", result);
  imshow("Gradiente", gradient);

  imwrite("gradiente.png", gradient);
  imwrite("imagem_com_gradiente.png", result);
  imwrite("entrada_q3d.png", img);

  waitKey(0);
  return 0;
}
