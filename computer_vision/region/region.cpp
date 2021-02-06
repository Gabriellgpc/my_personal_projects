#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

void region(Mat &img, CvPoint *p){
  for(unsigned int i = p[0].x; i < p[1].x; i++)
    for(unsigned int j = p[0].y; j < p[1].y; j++)
      img.at<uint8_t>(i,j) = 255 - img.at<uint8_t>(i,j);
}

int main(int argc, char** argv)
{
     Mat img;
     CvPoint p[2];

     img = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
     //if fail to read the image
     if(!img.data){
       std::cout << "imagem nao carregou corretamente\n";
       return(-1);
     }

     std::cout << "Dimensoes da imagem:" << img.size() << '\n';
     std::cout << "Informe o ponto 1 (coluna, linha):\t";
     std::cin >> p[0].y >> p[0].x;
     std::cout << "Informe o ponto 2 (coluna, linha):\t";
     std::cin >> p[1].y >> p[1].x;

     //show the image
     imshow("Original", img);
     region(img, p);
     imshow("Region Result", img);
     // Wait until user press some key
     waitKey(0);
     imwrite("resultados/region_result.png", img);
    return 0;
}
