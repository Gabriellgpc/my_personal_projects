// Programa para mesclar duas imagens
#include <opencv2/opencv.hpp>
using namespace cv;

const char* mainWindow = "Blending";

Mat imgA, imgB;
Mat imgC;

void blending(int, void*);

int main(int argc, char**argv)
{

  if(argc != 3)
  {
    std::cout << "Voce deve informar o caminho de duas imagens de mesmo tamanho\n";
    return(-1);
  }

  imgA = imread(argv[1],CV_LOAD_IMAGE_COLOR);
  imgB = imread(argv[2],CV_LOAD_IMAGE_COLOR);

  //Teste se as imagens foram carregadas corretamente
  if(!imgA.data){
    std::cout << "imagem "<< argv[1] <<"nao carregou corretamente\n";
    return(-1);
  }
  if(!imgB.data){
    std::cout << "imagem "<< argv[2] <<"nao carregou corretamente\n";
    return(-1);
  }
  //Testa se as imagens possuem o mesmo tamanho
  if(imgA.size() != imgB.size())
  {
    std::cout << "As imagens devem possuir o mesmo tamanho\n";
    return(-1);
  }

  imgC = imgB.clone();

  //Cria o slider, para ajustar a variavel alpha do calculo do blending
  namedWindow(mainWindow, WINDOW_AUTOSIZE);
  createTrackbar( "Alpha [0-100%]",  mainWindow,
                                     NULL,
                                     100,
                                     blending);
  int key;
  while(true)
  {
    imshow(mainWindow, imgC);
    key = waitKey(10);
    if(key == 27)//esq
      break;
  }
  return 0;
}

void blending(int pos, void*)
{
  float alpha = pos/100.0;
  for(int y = 0; y < imgA.rows; y++)
  for(int x = 0; x < imgA.cols; x++)
  {
      imgC.at<Vec3b>(y,x) = imgA.at<Vec3b>(y,x)*alpha + imgB.at<Vec3b>(y,x)*(1.0 - alpha);
  }
}
