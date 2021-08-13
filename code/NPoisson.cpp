#include "NPoisson.hpp"

Poisson::Poisson(const Mat& s, Mat& d, Mat& m) : src(s), dst(d), msk(m)
{
	// load image
	src.convertTo(src, CV_32F, 1 / 255.f);
	dst.convertTo(dst, CV_32F, 1 / 255.f);
	msk.convertTo(msk, CV_8UC1);
}

void Poisson::makeTable(int gradType = pixel::GRAD_IMPORTING) {
	table.clear();

	for (int y = 0; y < src.rows; y++) for (int x = 0; x < src.cols; x++) {
		if (msk.at<uchar>(y, x) == 255) {
			pixel tmp(x, y);
			tmp.neighbor(msk, gradType);
			tmp.gradient(src, dst, gradType);
			table.push_back(tmp);
		}
	}
}

Mat Poisson::run(void) {
	
	Mat fin = dst.clone();

	for (int iter = 0; iter < 100; iter++)
		for (pixel p : table)
			p.new_pixel(fin, fin, dst);

	return fin;
}