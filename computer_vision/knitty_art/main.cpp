#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include <string>
#include "artist.hpp"

using namespace std;
using namespace cv;

#define width_default 500
#define N_PINS 240
#define N_LINES 1000
#define MAX_IT  99999
#define MIN_DIST 100
#define MIN_SCORE 10
#define DECAY 100
#define DEBUG true

// #define width_default 500
// #define N_PINS 240
// #define N_LINES 1500
// #define MAX_IT  5000
// #define MIN_DIST 100
// #define MIN_SCORE 35
// #define DECAY 50
// #define DEBUG true

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
    
    int height_default = width_default*(img.size().height / img.size().width);
    cv::resize(img, img, Size(width_default, height_default));

    result = knitty_art(img, movements, N_PINS, N_LINES, MAX_IT, MIN_DIST, MIN_SCORE, DECAY, DEBUG);

    imshow("input", img);
    imshow("output", result);
    waitKey(0);

    cout << "[INFO] Salvando os resultados...\n";
    imwrite(image_file + string("_.png"), result);
    save_movements(movements, image_file + string("_.moves"));
    cout << "[INFO] Resultados salvos!\n";

    return 0;
}