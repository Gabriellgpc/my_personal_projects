#include <iostream>
#include <string>
#include <cstring>
#include "mycv.hpp"

using namespace std;
using namespace mycv;

int main(int argc, char **argv)
{
    Image img;

    cout << argc << " Argumentos \n"
         << argv[1] << '\n';

    cout << "carregando a imagem...\n";
    img = read_image(argv[1]);
    cout << "imagem carregada!\n";

    cout << "Propriedades da imagem:\n";
    cout << img.is_empty() << ' ' << img.width() << ' ' << img.height() << '\n';

    cout << "Salvando imagem...\n";
    write_image(img, "salvou?.pgm");
    cout << "Imagem salva!\n";

    return 0;
}