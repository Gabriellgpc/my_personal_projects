#include <iostream>
#include <string>
#include <cstring>
#include "mycv.hpp"

using namespace std;
using namespace mycv;

int main(int argc, char **argv)
{
    Image img;

    // Media
    uint16_t f = 10;
    float kernel[f * f];
    for (int i = 0; i < f * f; i++)
        kernel[i] = 1.0 / (f * f);

    // Gaussian
    // float kernel[] = {1/16.0, 2/16.0, 1/16.0,
    //                   2/16.0, 4/16.0, 2/16.0,
    //                   1/16.0, 2/16.0, 1/16.0};

    // Sobel
    // uint16_t f = 3;
    // float kernel[] = {-1, 0, 1,
    //                   -2, 0, 2,
    //                   -1, 0, 2};

    cout << argc << " Argumentos \n"
         << argv[1] << '\n';

    cout << "carregando a imagem...\n";
    img = read_image(argv[1]);
    cout << "imagem carregada!\n";
    cout << "Dimensões da imagem:\n";
    cout << img.width() << ' ' << img.height() << '\n';

    // img.padding(5, 5, 255);
    Image result = conv2D(img, kernel, f, 0, 1, false);
    cout << "Dimensões da imagem:\n";
    cout << result.width() << ' ' << result.height() << '\n';

    write_image(result, "teste.pgm");

    return 0;
}