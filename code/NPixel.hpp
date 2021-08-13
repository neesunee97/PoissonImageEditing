#ifndef NPixel_h
#define NPixel_h

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

struct pixel {

	//static const int GRAD_IMPORTING = 0;
	enum
	{
			GRAD_IMPORTING,
			GRAD_MIXING,
	};

	Point				p;
	vector<Point>		in_Np;
	vector<Point>		b_Np;
	Vec3f				grad;
	vector<Point>		nn;

	pixel	(int x, int y);

	void	neighbor	(const Mat& mask, int gradType);
	void	gradient	(const Mat& src, const Mat& dst, int flag = 0);
	Vec3f	gradMax		(const Point p, const Point n, const Mat& src, const Mat& dst);
	void	new_pixel	(const Mat& src, Mat& dst, const Mat& img);
};

#endif /* NPixel_h */