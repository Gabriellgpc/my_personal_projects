#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>  /* log2 */
#include <fstream> //ofstream
#include <vector>  //vector

using namespace cv;
double entropy(const Mat &imgGray)
{	
	double result = 0;
	uint64_t hist[256];

	memset(hist, 0 , 256*sizeof(uint64_t));
	
	// compute histogram
	for(int l = 0; l < imgGray.rows; l++)
	for(int c = 0; c < imgGray.cols; c++)
	{	
		hist[imgGray.at<uint8_t>(l,c)]++;
	}
	
	//computing the entropy
	double prob;
	double size = static_cast<double>(imgGray.rows * imgGray.cols);
	for(int i = 0; i < 256; i++)
	{
		prob = hist[i] / size;
		result += prob * log2(prob + (prob == 0));
	}

	return -result;
}

// The src image should be GRAYSCALE
void segmentation(const cv::Mat &src, cv::Mat &dest)
{
	cv::Mat srcTmp;
	static cv::Mat mask;
	std::vector<Vec3f> circles;
	Vec3f circle(0,0,0);
	float radius = 0;

	src.copyTo(srcTmp);
	
	// smooth it, otherwise a lot of false circles may be detected
	cv::GaussianBlur(src, src, cv::Size(3, 3), 1, 1);
	cv::HoughCircles(src, circles, cv::HOUGH_GRADIENT, 2, src.rows, 100, 100, src.cols/4, 0);

	// If more than one circle is detected pick up the the one with the biggest radiu
	if(circles.size() >= 1)
	{
		for(size_t i = 0; i < circles.size(); i++)
		{
			Vec3f c = circles[i];
			if(c[2] > radius)
			{
				radius = c[2];
				circle = c;
			}
		}
	}else{//try to use the last mask
		
		if(mask.empty())
			src.copyTo(dest);
		else //Aplying the mask			
			bitwise_and(srcTmp, mask, dest);
		// std::cerr << "[WARNING] Failed to find the circle\n";
		return;
	}
	// Creating the mask
	mask = cv::Mat::zeros(src.size(), CV_8UC1);
	cv::circle(mask, Point(circle[0], circle[1]), circle[2], 255, FILLED, LINE_AA);
	//Aplying the mask
	bitwise_and(srcTmp, mask, dest);
}

// src input image should be gray scale
// Outputs:
// Sobel in x == Sx
// Sobel in y == Sy
// Magnitude == sqrt( Sx^2 + Sy^2 )
void computeSobel(const cv::Mat &src, cv::Mat &Sx, cv::Mat &Sy, cv::Mat &mag)
{
	// 1. Sobel
	//  Sobel x
	cv::Sobel(src, Sx, CV_64FC1, 1, 0);
	//  Sobel y
	cv::Sobel(src, Sy, CV_64FC1, 0, 1);
	// 2. calculate gradient magnitude
	cv::magnitude(Sx, Sy, mag);
}

int main(int argc, char *argv[])
{
	std::ofstream fout("entropia.txt");
	VideoCapture video;
	Mat img, gray, Sx, Sy, mag;
	uint16_t frameCount = 0;

	video.open("HUV-03-14.wmv");
	if(video.isOpened() == false)
	{	
		std::cerr << "[ERROR] Failed to try open the video\n";
		return 1;
	}

	while(true)
	{
		if(!video.read(img))
		{
			break;
		}
		// convert from BGR to gray
		cvtColor(img, gray, COLOR_BGR2GRAY);
		// segmente central circle
		segmentation(gray, gray);
		// compute Sobel
		computeSobel(gray, Sx, Sy, mag);
		
		// Converting the gradient magnitude to 8U
		cv::convertScaleAbs(mag, mag);
		// compute entropy
		fout << frameCount << ' ' << entropy(gray) << ' ' << entropy(mag) << '\n';
		
		imshow("Input", img);
		imshow("Segmented", gray);
		imshow("Sobel", mag);
		if((waitKey(0) & 0xFF) == (uint8_t)'q')
		{
			break;
		}

		frameCount++;
	}
	fout.close();
	return 0;
}