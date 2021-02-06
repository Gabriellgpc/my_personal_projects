#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char** argv){
  Mat img;
  Mat result;
  int w, h;

  img = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
  //if fail to read the image
  if ( img.empty() )
  {
       cout << "Error loading the image" << endl;
       return -1;
  }

  w = img.size().width;
  h = img.size().height;
  result = img.clone();

  img(cv::Rect(0, 0, w/2, h/2)).copyTo(result(cv::Rect((w-1)/2, (h-1)/2, w/2, h/2)));
  img(cv::Rect((w-1)/2, 0, w/2, h/2)).copyTo(result(cv::Rect(0, (h-1)/2, w/2, h/2)));
  img(cv::Rect(0, (h-1)/2, w/2, h/2)).copyTo(result(cv::Rect((w-1)/2, 0, w/2, h/2)));
  img(cv::Rect((w-1)/2, (h-1)/2, w/2, h/2)).copyTo(result(cv::Rect(0, 0, w/2, h/2)));

  imshow("Original", img);
  imshow("Resultado", result);
  waitKey();

  imwrite("resultados/trocaRegiao_resultado.png", result);

  return 0;
}
