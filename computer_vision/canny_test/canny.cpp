#include <opencv2/opencv.hpp>
using namespace cv;

const char* mainWindow = "mainWindow";

int main(int argc, char**argv)
{
  Mat img, canny_img;
  int T1;
  int key;

  img = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
  if(!img.data){
    std::cerr << "Erro ao carregar a imagem!\n";
    return -1;
  }

  namedWindow(mainWindow, WINDOW_AUTOSIZE);
  createTrackbar( "T1",  mainWindow,
                                     &T1,
                                     200,
                                     NULL);

  imshow("Original", img);
  while(true)
  {
    Canny(img, canny_img, T1, 3*T1);
    imshow(mainWindow, canny_img);

    key = waitKey(10);
    if(key == 27)//esq
      break;
  }
  return 0;
}
