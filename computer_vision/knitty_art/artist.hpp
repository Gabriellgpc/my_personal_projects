#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <list>
#include <string>
#include <algorithm>

void save_movements(const std::list<cv::Point2i> &movements, const std::string file);

cv::Mat
knitty_art(const cv::Mat &img,
           std::list<cv::Point2i> &movements,
           const unsigned int &n_pins,
           const unsigned int &max_n_lines,
           const unsigned int &max_n_iterations,
           const unsigned int &min_dist = 10,
           const float &min_line_score = 0,
           const float &decay = 255,
           const bool debug = false);