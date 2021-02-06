//Programa que separa os três canais (RGB) e exibi uma imagem de cada canal
#include <opencv2/opencv.hpp>
using namespace cv;

int main(int argc, char**argv)
{
  Mat img;
  Mat ch[3];

  img = imread(argv[1],CV_LOAD_IMAGE_COLOR);

  if(!img.data){
    std::cout << "imagem nao carregou corretamente\n";
    return(-1);
  }
  imshow("Original", img);

  ch[0] = Mat(img.size(), CV_8UC3);
  ch[1] = Mat(img.size(), CV_8UC3);
  ch[2] = Mat(img.size(), CV_8UC3);

  for(int y = 0; y < img.rows; y++)
  for(int x = 0; x < img.cols; x++)
  {
    ch[0].at<Vec3b>(y,x)[0] = img.at<Vec3b>(y,x)[0];//B
    ch[1].at<Vec3b>(y,x)[1] = img.at<Vec3b>(y,x)[1];//G
    ch[2].at<Vec3b>(y,x)[2] = img.at<Vec3b>(y,x)[2];//R
  }

  imshow("Canal B", ch[0]);
  imshow("Canal G", ch[1]);
  imshow("Canal R", ch[2]);

  imwrite("canal_B.png", ch[0]);
  imwrite("canal_G.png", ch[1]);
  imwrite("canal_R.png", ch[2]);

  imwrite("entrada_q3a.png", img);

  waitKey(0);
  return 0;
}
