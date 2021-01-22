#pragma once
#define MY_PIX

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;
///////////////////////////////////////////////////////////////////////////////////////////////////
class Pix {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	std::vector<Point> contour;

	Rect boundingRect;

	Point centerPosition;

	double dblDiagonalSize;

	double dblAspectRatio;

	// function prototypes ////////////////////////////////////////////////////////////////////////
	Pix(std::vector<cv::Point> _contour);
};