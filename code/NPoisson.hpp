#ifndef NPoisson_h
#define NPoisson_h

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "NPixel.hpp"

using namespace std;
using namespace cv;

struct Poisson {

	Mat				src, dst, msk;
	vector<pixel>	table;

	Poisson(const Mat& sourceImg, Mat& destinationImg, Mat& mask);

	void makeTable(int gradType);

	Mat run(void);

};
#endif /*NPoisson_h*/