#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */

using namespace cv;
using namespace std;


Mat filter_tmp;
int percent_GamaL = 0;
int percent_GamaH = 2;
int percentD0 = 20;
int percent_C = 0;
float d_max;

void slot_sliders(int, void*);
void calcFilter(Mat& H, float gamaL, float gamaH, float D0, float c);
// troca os quadrantes da imagem da DFT
void deslocaDFT(Mat& image ){
  Mat tmp, A, B, C, D;

  // se a imagem tiver tamanho impar, recorta a regiao para
  // evitar cÃ³pias de tamanho desigual
  image = image(Rect(0, 0, image.cols & -2, image.rows & -2));
  int cx = image.cols/2;
  int cy = image.rows/2;

  // reorganiza os quadrantes da transformada
  // A B   ->  D C
  // C D       B A
  A = image(Rect(0, 0, cx, cy));
  B = image(Rect(cx, 0, cx, cy));
  C = image(Rect(0, cy, cx, cy));
  D = image(Rect(cx, cy, cx, cy));

  // A <-> D
  A.copyTo(tmp);  D.copyTo(A);  tmp.copyTo(D);

  // C <-> B
  C.copyTo(tmp);  B.copyTo(C);  tmp.copyTo(B);
}

int main(int argc, char**argv){
  const char mainWindow[] = "Filtro";
  const int max_slider = 100;
  Mat image;
  Mat padded, filter;
  Mat complexImage, complexImage_out;
  Mat_<float> realInput, zeros;
  int dft_M, dft_N;
  vector<Mat> planos;

  if(argc != 1){//usuario passou um argumento, vou entender como sendo um endereco de uma imagem
    image = imread(argv[1]);
    //converte para escala de cinza
    cvtColor(image, image, CV_BGR2GRAY);
    imshow("Entrada", image);
  }else{
    std::cerr << "Erro ao carregar imagem!" << '\n';
    return -1;
  }

  // identifica os tamanhos otimos para
  // calculo do FFT
  dft_M = getOptimalDFTSize(image.rows);
  dft_N = getOptimalDFTSize(image.cols);
  d_max = sqrt(dft_M*dft_M + dft_N*dft_N)/2.0;

  // realiza o padding da imagem
  copyMakeBorder(image, padded, 0,
                 dft_M - image.rows, 0,
                 dft_N - image.cols,
                 BORDER_CONSTANT, Scalar::all(0));

  // parte imaginaria da matriz complexa (preenchida com zeros)
  zeros = Mat_<float>::zeros(padded.size());
  // prepara a matriz complexa para ser preenchida
  complexImage = Mat(padded.size(), CV_32FC2, Scalar(0));

  // cria a componente real
  realInput = Mat_<float>(padded);
  realInput += cv::Scalar::all(1);
  cv::log(realInput, realInput);
  // combina o array de matrizes em uma unica
  // componente complexa
  Mat comps[] = {realInput, zeros};
  merge(comps, 2,  complexImage);
  // calcula o dft
  dft(complexImage, complexImage);
  // realiza a troca de quadrantes
  deslocaDFT(complexImage);
  normalize(complexImage, complexImage, 0, 1, CV_MINMAX);
  // a funcao de transferencia (filtro frequencial) deve ter o
  // mesmo tamanho e tipo da matriz complexa
  filter = complexImage.clone();

  // matriz das componentes real
  // e imaginaria do filtro
  filter_tmp = Mat(dft_M, dft_N, CV_32F);
  calcFilter(filter_tmp, percent_GamaL/100.0,
                        (percent_GamaH/100.0)*9+1,
                        (percentD0/100.0)*d_max,
                        (percent_C/100.0)*4+1);

  namedWindow(mainWindow, WINDOW_AUTOSIZE);

  createTrackbar( "Gama L [0-100%]", mainWindow,
                  &percent_GamaL,
                  max_slider,
                  slot_sliders);
  createTrackbar( "Gama H [0-100%]", mainWindow,
                  &percent_GamaH,
                  max_slider,
                  slot_sliders);
  createTrackbar( "D0 [0-100%]", mainWindow,
                  &percentD0,
                  max_slider,
                  slot_sliders);
  createTrackbar( "c [0-100%]", mainWindow,
                  &percent_C,
                  max_slider,
                  slot_sliders);

  complexImage_out = complexImage.clone();
  while(true){
    // cria a matriz com as componentes do filtro e junta
    // ambas em uma matriz multicanal complexa
    planos.clear();
    planos.push_back(filter_tmp);
    planos.push_back(filter_tmp);
    merge(planos, filter);

    // aplica o filtro frequencial
    mulSpectrums(complexImage, filter,complexImage_out,0);
    // troca novamente os quadrantes
    deslocaDFT(complexImage_out);
    // calcula a DFT inversa
    idft(complexImage_out, complexImage_out);
    // limpa o array de planos
    planos.clear();
    // separa as partes real e imaginaria da
    // imagem filtrada
    split(complexImage_out, planos);
    cv::exp(planos[0], planos[0]);
    planos[0] -= cv::Scalar::all(1);

    // normaliza a parte real para exibicao
    normalize(planos[0], planos[0], 0, 1, CV_MINMAX);

    imshow(mainWindow, filter_tmp);
    imshow("Resultado", planos[0]);
    waitKey(10);
  }

  return 0;
}

//cada slider eh mapeado de 0-100% em um range de valores
//fixos para cada uma das variaveis.
// (foi uma maneira simples que achei para controlar as variaveis com sliders de valores inteiros)
void slot_sliders(int, void*){
  calcFilter(filter_tmp, percent_GamaL/100.0,
                     (percent_GamaH/100.0)*8+2,
                     (percentD0/100.0)*d_max,
                     (percent_C/100.0)*4+1);
}

void calcFilter(Mat& H, float gamaL, float gamaH, float D0, float c){
  system("clear");
  std::cout << "Gama L:"<< gamaL << '\n';
  std::cout << "Gama H:"<< gamaH << '\n';
  std::cout << "D0:"<< D0 << '\n';
  std::cout << "C:"<< c << '\n';

  int N = H.size().width;
  int M = H.size().height;

  //Macro de calculo de distancia, com relacao ao ponto central da imagem
  #define D(u,v) sqrt( ((u)-M/2)*((u)-M/2) + ((v)-N/2)*((v)-N/2) )

  for(int u = 0; u < M; u++){
    for(int v = 0; v < N; v++){
        H.at<float>(u,v) = (gamaH - gamaL)*(1 - exp(-c*(D(u,v)*D(u,v))/(D0*D0))) + gamaL;
    }
  }
}
