#include <opencv2/opencv.hpp>
#include <cstdlib>

using namespace cv;

int main( int argc, char** argv ){
  int nClusters = 10;
  Mat rotulos;
  int nRodadas = 10;
  Mat centros;
  Mat resultados[nRodadas];

  if(argc!=2)exit(0);

  Mat img = imread( argv[1], CV_LOAD_IMAGE_COLOR);

  Mat samples(img.rows * img.cols, 3, CV_32F);

  //preenche o vetor de amostras
  for( int y = 0; y < img.rows; y++ ){
    for( int x = 0; x < img.cols; x++ ){
      for( int z = 0; z < 3; z++){
        samples.at<float>(y + x*img.rows, z) = img.at<Vec3b>(y,x)[z];
	  }
	 }
  }

  //Realiza nRodadas de calculo de kmeans, cada uma, muito provavelmente, retornara
  //um valor diferente para cada media represntativa, pra cada uma das nClusters classes
  for(uint i = 0; i < nRodadas; i++)
  {
    kmeans(samples,
      nClusters,
      rotulos,
      TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001),
      1,
      KMEANS_RANDOM_CENTERS,
      centros );

      Mat rotulada( img.size(), img.type() );
      for( int y = 0; y < img.rows; y++ ){
        for( int x = 0; x < img.cols; x++ ){
          int indice = rotulos.at<int>(y + x*img.rows,0);
          rotulada.at<Vec3b>(y,x)[0] = (uchar) centros.at<float>(indice, 0);
          rotulada.at<Vec3b>(y,x)[1] = (uchar) centros.at<float>(indice, 1);
          rotulada.at<Vec3b>(y,x)[2] = (uchar) centros.at<float>(indice, 2);
        }
      }
      //salva no vetor resultados, para posteriormente virarem um gif
      rotulada.copyTo(resultados[i]);
  }
  //salva cada iteraca em um arquivo png
  //para posteriormente virarem um gif
  //com a ferramenta imageMagick por meio do
  //do programa via terminal, convert.
  for(uint i = 0; i < nRodadas; i++)
    imwrite("rodada-"+std::to_string(i)+".png", resultados[i]);
}
