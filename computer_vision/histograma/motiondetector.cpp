#include <iostream>
#include <opencv2/opencv.hpp>

#include <GraphicsMagick/Magick++.h>

using namespace cv;
using namespace std;

#define NUM_FRAMES_GIF 7
#define GIF_DELAY 100 //ms

int main(int argc, char** argv){
  Magick::InitializeMagick(NULL);
  vector<Magick::Image> gifFrames(NUM_FRAMES_GIF);

  Mat frame, grayFrame;
  Mat H1, H2;
  VideoCapture cap;
  int i = 0;
  double r_correl = 0;

  int histsize = 256;
  float range[] = {0, 256};
  const float *histrange = { range };

  cap.open(0);

  if(!cap.isOpened()){
    cout << "cameras indisponiveis";
    return -1;
  }

  cap >> frame; //captura um frame
  cvtColor(frame, frame, CV_BGR2GRAY); //converte para escala de cinza
  calcHist(&frame, 1, 0, Mat(), H1, 1, &histsize, &histrange);

  while(true){
    H2 = H1.clone();
    cap >> frame; //captura um frame
    cvtColor(frame, grayFrame, CV_BGR2GRAY); //converte para escala de cinza
    calcHist(&grayFrame, 1, 0, Mat(), H1, 1, &histsize, &histrange);

    r_correl =  compareHist(H1, H2, CV_COMP_CORREL);

    if(r_correl <= 0.95)//movimento
    {
      circle(frame, Point(frame.cols - 20, 20), 10, Scalar(0, 255, 0), CV_FILLED);
    }else{
      circle(frame, Point(frame.cols - 20, 20), 10, Scalar(255, 255 ,255), CV_FILLED);
    }

    imshow("Live", frame);

    //Salva o frame para o array de frames que sera usado para gerar o gif
    gifFrames[i] = Magick::Image(frame.cols,
                                 frame.rows, "BGR",
                                 Magick::StorageType::CharPixel,
                                (uint8_t*)frame.data);
    gifFrames[i].animationDelay(GIF_DELAY);
    i = (i+1)%NUM_FRAMES_GIF; //para simular uma fila circular com o vector

    if(waitKey(30) != 255)break;
  }

  //Gera o gif, colocando-o no arquivo "saidaKmeans.gif"
  Magick::writeImages(gifFrames.begin(), gifFrames.end(), "Motiondetector.gif");
  return 0;
}
