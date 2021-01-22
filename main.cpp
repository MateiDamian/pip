#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>

#include "Pix.h"

using namespace cv;

const Scalar SCALAR_BLACK = Scalar(0.0, 0.0, 0.0);
const Scalar SCALAR_WHITE = Scalar(255.0, 255.0, 255.0);
const Scalar SCALAR_BLUE = Scalar(255.0, 0.0, 0.0);
const Scalar SCALAR_GREEN = Scalar(0.0, 255.0, 0.0);
const Scalar SCALAR_RED = Scalar(0.0, 0.0, 255.0);

int main(void) {

	VideoCapture capVideo;
	//Inseram videoclipul
	capVideo.open("C:\\Users\\Cristina\\Desktop\\ANU 3\\PIP\\test.mp4");

	if (!capVideo.isOpened()) {                                            
		std::cout << "\n error reading video file" << std::endl << std::endl;
		return(0);
	}

	char CheckEsc = 0;

	while (capVideo.isOpened() && CheckEsc != 27) {
		// Declaram doua frame-uri consecutive
		Mat Frame1;
		Mat Frame2;
		capVideo.read(Frame1);
		capVideo.read(Frame2);

		//Afisam frame2, frame ul original
		imshow("Frame Original", Frame2); 

		if (!capVideo.read(Frame1)) {
			break;
		}
		if (!capVideo.read(Frame2))
		{
			break;
		}

		std::vector<Pix> Pixs;

		Mat Frame1Copy = Frame1.clone();
		Mat Frame2Copy = Frame2.clone();

		Mat Difference;
		Mat Thresh;

		// Convertim Frame1 si Frame2 in HSV
		cvtColor(Frame1Copy, Frame1Copy, COLOR_BGR2HSV);
		cvtColor(Frame2Copy, Frame2Copy, COLOR_BGR2HSV);

		// Afisam Frame1 si Frame2 in HSV
		imshow("Frame 1 in HSV", Frame1Copy);
		//imshow("Frame 2 in HSV", Frame2Copy);

		// Identificam culoarea focului 
		inRange(Frame1Copy, Scalar(0, 0, 210), Scalar(70, 190, 255), Frame1Copy);
		inRange(Frame2Copy, Scalar(0, 0, 210), Scalar(70, 190, 255), Frame2Copy);

		// Afisam
		imshow("HSV Range Threshold Fire Frame 1", Frame1Copy);
		//imshow("HSV Range Threshold Fire Frame 2", Frame2Copy);

		// Comparam diferentele dintre cele doua frame uri
		absdiff(Frame1Copy, Frame2Copy, Difference);

		threshold(Difference, Thresh, 30, 255.0, cv::THRESH_BINARY);

		//imshow("Threshold", Thresh);



		Mat structuringElement = getStructuringElement(MORPH_RECT, Size(5, 5));
		dilate(Thresh, Thresh, structuringElement);
		dilate(Thresh, Thresh, structuringElement);
		erode(Thresh, Thresh, structuringElement);
		
		//Contur
		Mat ThreshCopy = Thresh.clone();
		std::vector<std::vector<Point> > contours;
		//Definire contur
		findContours(ThreshCopy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);	
		Mat Contours(Thresh.size(), CV_8UC3, SCALAR_BLACK);
		//Desenare contur
		drawContours(Contours, contours, -1, SCALAR_WHITE, -1);
		//imshow("Contours", Contours);

		//
		std::vector<std::vector<Point> > convexHulls(contours.size());
		for (unsigned int i = 0; i < contours.size(); i++) {
			convexHull(contours[i], convexHulls[i]);	//Convex Hull
		}
		for (auto& convexHull : convexHulls)
		{
			Pix possiblePix(convexHull);
			if (possiblePix.boundingRect.area() > 50 &&
				possiblePix.dblAspectRatio >= 0.2 &&
				possiblePix.dblAspectRatio <= 1.2 &&
				possiblePix.boundingRect.width > 15 &&
				possiblePix.boundingRect.height > 20 &&
				possiblePix.dblDiagonalSize > 30.0)
				Pixs.push_back(possiblePix);
		}

		Mat ConvexHulls(Thresh.size(), CV_8UC3, SCALAR_BLACK);
		convexHulls.clear();
		for (auto& Pix : Pixs) {
			convexHulls.push_back(Pix.contour);
		}
		drawContours(ConvexHulls, convexHulls, -1, SCALAR_WHITE, -1);
		imshow("ConvexHulls", ConvexHulls);


		Frame2Copy = Frame2.clone();         
		for (auto& Pix : Pixs) {
			//Desenam chenarul albastru in jurul focului
			rectangle(Frame2Copy, Pix.boundingRect, SCALAR_BLUE, 2);         
			//Desenam punctul rosu in mijlocul chenarului 
			circle(Frame2Copy, Pix.centerPosition, 3, SCALAR_GREEN, -1);        
		}

		// Afisam frame ul 2 (frame-ul cu chenarele)
		imshow("Frame2Copy", Frame2Copy);

		
		if ((capVideo.get(cv::CAP_PROP_POS_FRAMES) + 1) < capVideo.get(cv::CAP_PROP_FRAME_COUNT)) 
		{
			capVideo.read(Frame2);
		}
		else {
			std::cout << "Sfarsitul videoclipului \n";
			break;
		}
		CheckEsc = waitKey(1);
	}
	if (CheckEsc != 27) {
		waitKey(0);
	}
	return(0);
}