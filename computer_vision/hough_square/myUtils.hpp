// #pragma once
#ifndef __MY_UTILS_H__
#define __MY_UTILS_H__

#include <cstdint>
#include <opencv2/opencv.hpp>
#include <list>

// #define _DEBUG_LVL1_

namespace myUtils
{
    struct circle_t
    {
        double xc, yc;
        double radius;
    };

    class MySquare
    {
    public:
        MySquare() : _xc(0), _yc(0), _size(0), _theta(0), _votes(0) {}
        MySquare(const uint16_t &xc,
                 const uint16_t &yc,
                 const double &theta,
                 const uint16_t &size) : _xc(xc), _yc(yc), _size(size), _theta(theta), _votes(0) {}
        MySquare(const uint16_t &xc,
                 const uint16_t &yc,
                 const double &theta,
                 const uint16_t &size,
                 const uint16_t &votes) : _xc(xc), _yc(yc), _size(size), _theta(theta), _votes(votes) {}
        inline cv::Point2f center() const { return cv::Point2f((float)_xc, (float)_yc); }
        inline uint16_t &xc() { return _xc; }
        inline uint16_t &yc() { return _yc; }
        inline uint16_t &size() { return _size; }
        inline double &theta() { return _theta; }
        uint16_t isize(const uint16_t &min_square_size,
                       const uint16_t &max_square_size) const;
        //devolve o inteiro correspondente ao angulo no intervalo [0,90)
        uint16_t itheta(const uint16_t &ang_resolution);
        inline uint16_t &votes() { return _votes; }

    private:
        uint16_t _xc, _yc;
        uint16_t _size;
        double _theta; //angulo com relação ao eixo horizontal
        uint16_t _votes;
    };

    void convFilter(const cv::Mat &src, cv::Mat &dest, const cv::Mat &mask);
    void hough4circles(const cv::Mat &img, const cv::Mat &edge, std::list<circle_t> &circles, const uint32_t &max_circles, const uint &inf, const uint &sup);
    void drawCircles(cv::Mat &img, const std::list<circle_t> &circles);
    bool testCircleIsOutside(const cv::Mat &img, const circle_t &c, const uint8_t &background);
    //img: imagem com fundo branco e quadrados pretos
    //max_detection: -1 para tentar achar todos, ou um número inteiro para limitar
    std::list<MySquare>
    myHoughtTransform4Squares(const cv::Mat &img,
                              const int &max_detection,
                              const uint16_t &min_square_size,
                              const uint16_t &max_square_size,
                              const uint16_t &ang_resolution,
                              const float &edge_threshold,
                              const uint16_t &min_number_votes,
                              const uint16_t &max_number_votes);

    // std::list<MySquare> myHoughtTransform4Squares(const cv::Mat &img, const int &max_detection = -1);
    void drawSquares(const cv::Mat &src, cv::Mat &dest, const std::list<MySquare> &squares);
    void _drawSquare(const cv::Mat &src, cv::Mat &dest, const MySquare square);
    void squares_supression(std::list<MySquare> &squares);

}; // namespace myUtils

#endif