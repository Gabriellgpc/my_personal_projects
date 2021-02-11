#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <list>
#include <string>
#include <algorithm>

#include "artist.hpp"

// #define SAVE
#define FOLDER "steps"

std::vector<cv::Point2i>
_generate_pin_list(const unsigned int &n_pins,
                   const unsigned int radius,
                   cv::Point2i center)
{
    std::vector<cv::Point2i> pins;
    float ang_step = 2.0 * M_PI / n_pins;

    cv::Point2i pt;
    for (unsigned int i = 0; i < n_pins; i++)
    {
        pt.x = static_cast<int>(std::round(center.x + radius * std::cos(ang_step * i)));
        pt.y = static_cast<int>(std::round(center.y + radius * std::sin(ang_step * i)));
        pins.push_back(pt);
    }

    return pins;
}

//Calcula a quantidade de pixels pretos a linha vai cobrir/passar por cima
float _get_line_score(const cv::Mat &image,
                      const cv::Point2i p1,
                      const cv::Point2i p2,
                      unsigned int min_dist)
{
    float score = 0;
    float dist = 1;

    cv::LineIterator line_it(image, p1, p2);
    dist = line_it.count;

    if (dist <= min_dist)
        return score;

    for (int i = 0; i < line_it.count; i++, ++line_it)
    {
        score += 255.0 - image.at<uint8_t>(line_it.pos());
    }

    return score / dist;
}

void _reduce_image_ref(cv::Mat &image,
                       const cv::Point2i p1,
                       const cv::Point2i p2,
                       const int &decay)
{
    cv::LineIterator line_it(image, p1, p2);
    for (int i = 0; i < line_it.count; i++, ++line_it)
    {
        image.at<uint8_t>(line_it.pos()) = std::min(image.at<uint8_t>(line_it.pos()) + decay, 255);
    }
}

cv::Mat
knitty_art(const cv::Mat &img,
           std::list<cv::Point2i> &movements,
           const unsigned int &n_pins,
           const unsigned int &max_n_lines,
           const unsigned int &max_n_iterations,
           const unsigned int &min_dist,
           const float &min_line_score,
           const float &decay,
           const bool debug)
{
    //Imagem de saida, imagem que será criada apenas com linhas
    cv::Mat result = cv::Mat::ones(img.size(), CV_8UC1) * 255;
    //Imagem que servira de referencia (gray scale da imagem de entrada)
    cv::Mat img_ref = cv::Mat(img.size(), CV_8UC1);
    cv::Mat img_tmp;

    // Garante que a imagem result esteja em escala de cinza
    if (img.channels() > 1)
        cv::cvtColor(img, img_ref, cv::COLOR_BGR2GRAY);

    //raio do maior circulo dentro da imagem
    unsigned int radius = cv::min(img_ref.rows, img_ref.cols) / 2;
    //centro da imagem
    cv::Point2i center(img_ref.cols / 2, img_ref.rows / 2);

    /**segmenta a imagem, deixando o que estiver fora do circulo na cor branca e preservando o que estiver*/
    /**dentro (branco == background)**/
    //criando mascara: imagem com circulo "preenchido de preto" e seu exterior branco (background)
    cv::Mat mask = cv::Mat::ones(img_ref.size(), CV_8UC1) * 255;
    cv::circle(mask, center, radius, 0, cv::FILLED);
    //aplicando a mascara para segmentar a região de interesse

    cv::bitwise_or(img_ref, mask, img_ref);

    if (debug)
    {
        cv::resize(img_ref, img_tmp, cv::Size(320, 320*img_ref.size().height/(float)img_ref.size().width));
        cv::imshow("Reference Image", img_tmp);
        #ifdef SAVE
        cv::imwrite("reference_image.png", img_ref);
        #endif
        cv::waitKey(1);
    }

    //vetor de "pinos"
    //vetor que armazena as coordenadas de pixel de todos os pinos
    auto pins_vector = _generate_pin_list(n_pins, radius, center);

    movements.clear();
    unsigned int it = 0;
    unsigned int line_count = 0;
    //pino inicial
    unsigned int start_pin = 0;
    //melhor pino de chegada saindo de start_pin (maior score)
    unsigned int best_end_pin = 0;
    float best_end_score = 0.;
    float line_score;
    while (true)
    {
        // Teste de condição de parada
        if (it >= max_n_iterations || line_count >= max_n_lines)
            break;
        start_pin = best_end_pin;

        //busca o melhor pino de chegada saindo de start_pin
        best_end_score = 0;
        for (unsigned int end_pin = 0; end_pin < n_pins; end_pin++)
        {
            if (end_pin == start_pin)
                continue;
            line_score = _get_line_score(img_ref,
                                         pins_vector[start_pin],
                                         pins_vector[end_pin],
                                         min_dist);
            if (line_score > best_end_score)
            {
                best_end_score = line_score;
                best_end_pin = end_pin;
            }
        }

        it++;
        //caso a melhor pontuação (melhor linha) esteja abaixo do mínimo definido,
        //selecione outro best_end_pin apenas para trocar o start_pin quando voltar para o
        //inicio do loop
        if (best_end_score <= min_line_score)
        {
            best_end_score = (best_end_pin + 1) % n_pins; //vai alterar o start_pin
            continue;
        }

        //apaga a região correspondente a linha na imagem de referencia
        _reduce_image_ref(img_ref, pins_vector[start_pin], pins_vector[best_end_pin], decay);
        //adiciona o movimento ao historico de movimentos
        movements.push_back(cv::Point(start_pin, best_end_pin));
        line_count++;

        //desenha a reta na imagem resultante
        cv::line(result, pins_vector[start_pin], pins_vector[best_end_pin], 30, 1, cv::LINE_AA);

        if (debug)
        {

            //pressionar s vai ignorar o imshow e deve diminuir o delay no modo debug causado pelo imshow
            //mas obviamente vai pular a visualização de etapas do desenho
            if (cv::waitKey(1) == 's')
                continue;

            //mostrando resultados intermediarios
            // cv::imshow("Result", result);
            // cv::imshow("Reference", img_ref);
            cv::hconcat(img_ref, result, img_tmp);
            cv::resize(img_tmp, img_tmp, cv::Size(640, 640*img_tmp.size().height/(float)img_tmp.size().width));
            // cv::resize(img_tmp, img_tmp, cv::Size(), 0.5, 0.5);
            cv::imshow("Computing...", img_tmp);

            #ifdef SAVE
            if(it % 10 == 0)
                cv::imwrite(std::string(FOLDER)+"/step_"+std::to_string(it)+".png",img_tmp);
            #endif
        }
    }

    if (it >= max_n_iterations)
    {
        std::cout << "[INFO] Máximo número de iterações atingida!\n";
    }

    if (line_count >= max_n_lines)
    {
        std::cout << "[INFO] Máximo número de fios usados!\n";
    }

    return result;
}

void save_movements(const std::list<cv::Point2i> &movements, const std::string file)
{
    std::ofstream O(file);

    if(O.is_open() == false)
        return;
    
    for(auto it = movements.begin(); it != movements.end(); it++)
    {
        O << "FROM " << it->x << " TO " << it->y << '\n';
    }

    O.close();
}