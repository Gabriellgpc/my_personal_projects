#include <iostream>
#include <math.h>       /* tanh, log */
#include <opencv2/opencv.hpp>

using namespace cv;

bool tiltShift(Mat &image_src,
               Mat &image_dest,
               float delta_l, //0 to 1
               float decay,   //0 to 1
               float center,  //0 to 1
               float blurring);

void slot_sliders(int, void*);
double alfa_func(int x, int l1, int l2, double d);

Mat image, blender;
int decay_int = 50;
int center_int = 50;
int c_width_int = 50;
int blurring_int = 50;
const char mainWindow[] = "TiltSfhit";


int main(int argc, char** argv){
  const int max_slider = 100;

  if(argc != 1){//usuario passou um argumento, vou entender como sendo um endereco de uma imagem
    image = imread(argv[1]);
  }else{
    image = imread("imagens/toto_bola_cores.png");
  }

  if(image.data == NULL){
    std::cerr << "Erro ao abrir a Imagem!!" << '\n';
    return 1;
  }

  namedWindow(mainWindow, WINDOW_AUTOSIZE);
  imshow(mainWindow, image);

  createTrackbar( "Central Width [0,100]", mainWindow,
                  &c_width_int,
                  max_slider,
                  slot_sliders);

  createTrackbar( "Center [0,100]", "TiltSfhit",
        				  &center_int,
        				  max_slider,
        				  slot_sliders);

  createTrackbar( "Decay [0,100]", mainWindow,
        				  &decay_int,
        				  max_slider,
        				  slot_sliders);

  createTrackbar( "Blurring [0,100]", mainWindow,
                  &blurring_int,
                  max_slider,
                  slot_sliders);

  slot_sliders(0,0);

  waitKey(0);
  return 0;
}

void slot_sliders(int, void*){
  tiltShift(image, blender, c_width_int/100.0, decay_int/100.0, center_int/100.0, blurring_int/100.0);
  imshow(mainWindow, blender);
};

double alfa_func(int x, int l1, int l2, double d){
  return 0.5*( tanh((x-l1)/d) - tanh((x-l2)/d) );
}

bool tiltShift(Mat &image_src,
               Mat &image_dest,
               float delta_l, //0 to 1
               float decay,   //0 to 1
               float center,  //0 to 1
               float blurring)
{
  if(delta_l > 1 || center > 1 || decay > 1 || blurring > 1)return true;
  if(delta_l < 0 || center < 0 || decay < 0 || blurring < 0)return true;
  if(image_src.data == NULL)return true;

  const static int intensity_max = 100;
  Mat blurry_img;
  const int x_max = image_src.rows;
  Mat mask, image_32f;
  float weigth = 1/18.0;
  float average[] = {2, 2, 2, 2, 2, 2, 2, 2, 2};


  mask = Mat(3, 3, CV_32F, average);
  scaleAdd(mask, weigth, Mat::zeros(3,3,CV_32F), mask);
  image_src.convertTo(image_32f, CV_32F);
  for(int i = 0; i < intensity_max*blurring; i++)
  {
    filter2D(image_32f, image_32f, image_32f.depth(), mask, Point(1,1));
    image_32f = abs(image_32f);
  }
  image_32f.convertTo(blurry_img, CV_8U);

  #define L1 (center - delta_l/2.0)*x_max
  #define L2 (center + delta_l/2.0)*x_max
  image_dest = image_src.clone();
  double alfa;
  for(int x = 0; x < x_max; x++)
  {
      alfa = alfa_func(x, L1, L2, decay);
      addWeighted(  image_src.row(x),
                    alfa,
                    blurry_img.row(x),
                    1-alfa,
                    0.0,
                    image_dest.row(x));
  }

  return false;
};
