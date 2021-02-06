#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat frame;
  Mat hist;
  VideoCapture cap;
  uint8_t histEq[256];

  int histsize = 256;
  int sum;
  float range[] = {0, 256};
  const float *histrange = { range };

  cap.open(0);

  if(!cap.isOpened()){
    cout << "cameras indisponiveis\n";
    return -1;
  }

  std::cout << "Pressione qualquer tecla para encerrar o programa." << '\n';
  while(1){
    cap >> frame;

    cvtColor(frame, frame, CV_BGR2GRAY);
    imshow("Original", frame);
    //Calculo do histograma
    calcHist(&frame, 1, 0, Mat(), hist, 1, &histsize, &histrange);


    /*calculo do histograma acumulado */
    /*e normalizacao do histograma acumulado*/
    /**
     * Calcula o vetor que ira realizar a transformacao nos valores dos pixels
     */
    sum = 0;
    for(int i = 0; i < histsize; i++)
    {
      sum+= hist.at<float>(i);
      histEq[i] = sum*255.0/frame.total();
    }

    //substituicao dos valores dos pixels
    for(int i = 0; i < frame.size().height; i++)
      for(int j = 0; j < frame.size().width; j++)
        frame.at<uint8_t>(i,j) = histEq[frame.at<uint8_t>(i,j)];
    imshow("Equalizado", frame);

    if(waitKey(10) != 255)break;
  }

  return 0;
}
