#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include <string>
#include "artist.hpp"

using namespace std;
using namespace cv;

#define width_default 640*2
#define N_PINS 240
#define N_LINES 3000
#define MAX_IT  N_LINES*2
#define MIN_DIST width_default*0.01
#define MIN_SCORE 30
#define DECAY 100
#define DEBUG true

int main(int argc, char** argv)
{
    Mat img;
    Mat result;
    std::list<cv::Point2i> movements;


    if(argc != 2)
    {   
        cerr << "[ERROR] Faltou informar o caminho da imagem de entrada!\n";
        return -1;
    }
    std::string image_file(argv[1]);
    img = imread(image_file, IMREAD_COLOR);
    
    int height_default = width_default*(img.size().height /(float)img.size().width);
    cv::resize(img, img, Size(width_default, height_default));

    cout << "[INFO] Artista trabalhando...\n";
    result = knitty_art(img, movements, N_PINS, N_LINES, MAX_IT, MIN_DIST, MIN_SCORE, DECAY, DEBUG);
    cout << "[INFO] Obra finalizada!\n";

    if(DEBUG) waitKey(0);

    cout << "[INFO] Salvando os resultados...\n";
    imwrite(image_file + string("_.png"), result);
    save_movements(movements, image_file + string("_.moves"));
    cout << "[INFO] Resultados salvos!\n";

    return 0;
}