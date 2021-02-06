/***********************************************************************************************/
/******* PROGRAMA PARA REALIZAR SUAVIZAÇÃO SELETIVA DE IMAGENS BASEADO DETECÇÃO DE BORDA *******/
/* O PROGRAMA APLICA UMA SUAVIZAÇÃO FORTE NOS PIXEIS QUE NÃO SÃO CLASSIFICADOS COMO PIXEL DE BORDA */
/* E UMA SUAVIZAÇÃO SUAVE CASO CONTRARIO */
/***********************************************************************************************/
#include <string>
#include <opencv2/opencv.hpp>

// using namespace cv;
using namespace std;

#define MEDIA_SIZE 10.0

void convFilter(const cv::Mat &src, cv::Mat &dest, const cv::Mat &mask);
void selectiveFiltering(const cv::Mat &src, cv::Mat &dest, const float &edge_threshold = 200);

int main(int argc, char **argv)
{
    cv::Mat img, result;
    float edge_threshold;

    if(argc != 3)
    {
        std::cout << "Quantidade de argumentos inferior ao esperado\n";
        std::cout << "Entrada esperada:\n";
        std::cout << "./q1 imagem_path edge_threshold\n";
        return -1;
    }
    
    edge_threshold = fabs(std::stof(argv[2]));
    
    img = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    if (!img.data)
    {
        std::cerr << "Erro ao carregar imagem\n";
        return -1;
    }
    
    std::cout << "Aplicando o filtro...\n";
    selectiveFiltering(img, result, edge_threshold);
    std::cout << "Filtragem concluida!\n";

    //trecho apenas para exibir e salvar as imagens
    cv::imshow("Entrada", img);
    cv::imshow("Saida", result);
    cv::imwrite("entrada_q1.png", img);
    cv::imwrite("saida_q1.png", result);

    cv::waitKey(0);
    return 0;
}

void selectiveFiltering(const cv::Mat &src, cv::Mat &dest, const float &edge_threshold)
{
    cv::Mat gx, gy, g_mod;
    cv::Mat mask, blurImg;
    float xSobel[] = {-1, 0, 1, 
                      -2, 0, 2, 
                      -1, 0,1};
    float ySobel[] = {1, 2, 1, 
                      0, 0, 0,
                     -1, -2, -1};
    float kernel_media[(int)(MEDIA_SIZE * MEDIA_SIZE)];
    float kernel_gaussian[] = {1, 2, 1, 2, 4, 2, 1, 2, 1};

    /***********************************************/
    /*Aplicar suavização Gaussiana em toda a imagem*/
    /***********************************************/
    mask = cv::Mat(3, 3, CV_32F, kernel_gaussian) / 16.0;
    convFilter(src, dest, mask);
    dest.convertTo(dest, CV_8U);
    cv::imshow("Imagem suavizada levemente", dest);
    cv::imwrite("leve_suavizacao_q1.png", dest);
    /********************************/
    /*Calcular o gradiente da imagem*/
    /********************************/
    
    //gradiente na direção x ->(right)
    mask = cv::Mat(3, 3, CV_32F, xSobel);
    convFilter(dest, gx, mask);
    //gradiente na direção y (up)
    mask = cv::Mat(3, 3, CV_32F, ySobel);
    convFilter(dest, gy, mask);
    //modulo da matriz gradiente
    g_mod = gx.mul(gx) + gy.mul(gy);
    cv::sqrt(g_mod, g_mod);

    //Apenas para exibir a imagem do gradiente
    gx.convertTo(gx, CV_8U, 255.0 / 2040.0, 127.0);
    gy.convertTo(gy, CV_8U, 255.0 / 2040.0, 127.0);
    cv::imshow("Gx", gx);
    cv::imshow("Gy", gy);
    cv::imwrite("gx_q1.png",gx);
    cv::imwrite("gy_q1.png",gy);

    gx = cv::Mat();
    gy = cv::Mat();
    /******************/
    /*Suavização forte*/
    /******************/
    //criar o kernel
    for (uint i = 0; i < (MEDIA_SIZE * MEDIA_SIZE); i++)
        kernel_media[i] = 1.0 / (MEDIA_SIZE * MEDIA_SIZE);
    //aplicar o filtro de borramento
    mask = cv::Mat(MEDIA_SIZE, MEDIA_SIZE, CV_32F, kernel_media);
    convFilter(dest, blurImg, mask);
    blurImg.convertTo(blurImg, CV_8U);
    
    cv::imshow("Imagem Borrada", blurImg);
    cv::imwrite("borrada_q1.png", blurImg);
    /*********************************************************/
    /*Sobrepor a imagem borrada sobre as regioes de não borda*/
    /*********************************************************/
    for(uint16_t y = 0; y < src.size().height; y++)
    for(uint16_t x = 0; x < src.size().width; x++)
    {   
        //se não for pixel de bordar: usar resultado da filtragem forte
        if(fabs(g_mod.at<float>(y,x)) < edge_threshold)
        {
            dest.at<uint8_t>(y,x) = blurImg.at<uint8_t>(y,x);
        }
    }

    /******************************************/
    /*Converte a imagem de destino para uin8_t*/
    /******************************************/
    dest.convertTo(dest, CV_8U);
}

void convFilter(const cv::Mat &src, cv::Mat &dest, const cv::Mat &mask)
{
    float sum;
    cv::Mat src_f;

    src.convertTo(src_f, CV_32F);
    dest = cv::Mat::zeros(src.size(), CV_32F);

    for (int y = mask.rows / 2; y < (src.rows - mask.rows / 2); y++)
        for (int x = mask.cols / 2; x < (src.cols - mask.cols / 2); x++)
        {
            sum = 0;
            for (int v = 0; v < mask.rows; v++)
                for (int u = 0; u < mask.cols; u++)
                {
                    sum += src_f.at<float>(y - mask.rows / 2 + v, x - mask.cols / 2 + u) * mask.at<float>(v, u);
                }
            dest.at<float>(y, x) = sum;
        }
}
