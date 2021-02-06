#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace cv;

#define STEP 7
#define JITTER 3
#define RAIO_MAX 2

//funcao que desenha circulos de raio deteminados
//nas bordas destacadas pelo algoritmo de canny
//usando T1 e T2 = 3*T1, como limites infeior e superior
//respectivamente
//Os circulos sao desenhados em ordem aleatoria, para evitar efeitos padronizados
//e as cores dos circulos sao versoes levemente diferentes das tonalidades originais
//para dar efeito de erro humano
void pontilhar(const Mat &imgIn, Mat &imgOut,uint raio, uint T1);

int main(int argc, char** argv){
  vector<int> yrange;
  vector<int> xrange;

  Mat image, points, canny_img;

  uint width, height;
  uint x, y;

  image= imread(argv[1]);
  imshow("original", image);

  srand(time(0));

  if(!image.data){
    cout << "nao abriu" << argv[1] << endl;
    exit(0);
  }

  width=image.size().width;
  height=image.size().height;

  xrange.resize(height/STEP);
  yrange.resize(width/STEP);

  iota(xrange.begin(), xrange.end(), 0);
  iota(yrange.begin(), yrange.end(), 0);

  for(uint i=0; i<xrange.size(); i++){
    xrange[i]= xrange[i]*STEP+STEP/2;
  }

  for(uint i=0; i<yrange.size(); i++){
    yrange[i]= yrange[i]*STEP+STEP/2;
  }

  points =  Mat(height, width, CV_8UC3, Scalar::all(255));

  random_shuffle(xrange.begin(), xrange.end());
  for(auto i : xrange){
    random_shuffle(yrange.begin(), yrange.end());
    for(auto j : yrange){
      x = i+rand()%(2*JITTER)-JITTER +1;
      y = j+rand()%(2*JITTER)-JITTER +1;

      Vec3b val = image.at<Vec3b>(x,y);
      if(x >= height || y >= width)val = Vec3b(255,255,255);

      circle(points,
             cv::Point(y,x),
             RAIO_MAX+(rand()%2),
             val,
             -1,
             CV_AA);
    }
  }

  //3 tipos de tracos sao aplicados aqui
  //a medida que o limite inferior eh menor, mais bordas sao detectadas
  //e essas bordas serao "pintadas" com "pinceis mais grossos"
  //e para as bordas de menor limiar, "pinceis mais finos" sao usados
  pontilhar(image, points, 3, 1);
  pontilhar(image, points, 2, 20);
  pontilhar(image, points, 1, 80);

  std::string nomeArq = std::string(argv[1]);
  nomeArq = "Pontilhismo_"+nomeArq;
  imwrite(nomeArq, points);
  std::cout << "Resultado salvo em: " << nomeArq << '\n';
  return 0;
}

void pontilhar(const Mat &imgIn, Mat &imgOut, uint raio, uint T1){
  std::vector<int> xrange(imgIn.rows);
  std::vector<int> yrange(imgIn.cols);
  cv::Mat canny_img;
  uint x, y;

  iota(xrange.begin(), xrange.end(), 0);
  iota(yrange.begin(), yrange.end(), 0);

  //Destacando as bordas com o filtro de Canny
  cvtColor(imgIn, canny_img, CV_BGR2GRAY);
  Canny(canny_img, canny_img, T1, 3*T1);

  random_shuffle(xrange.begin(), xrange.end());
  for(auto i : xrange){
    random_shuffle(yrange.begin(), yrange.end());
    for(auto j : yrange){
      if(canny_img.at<uint8_t>(i,j) == 255){
        x = i+rand()%(4*raio)-1.5*raio + 1;
        y = j+rand()%(4*raio)-1.5*raio + 1;
        circle(imgOut,
          cv::Point(y,x),
          raio,
          imgIn.at<Vec3b>(i,j)*(1+(rand()%3)/10.0),  //realiza pequenas mudanças nas tonalidades originais, para dar efeito de erro humano
          -1,
          CV_AA);
      }
    }
  }
}
