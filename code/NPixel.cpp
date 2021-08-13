#include "NPixel.hpp"

pixel::pixel(int x, int y) : p(x, y) {
	in_Np.clear();
	b_Np.clear();
	nn.clear();

	grad = Vec3f(0, 0, 0);

	nn = {	Point(p.x, p.y - 1), 
			Point(p.x + 1, p.y),
			Point(p.x, p.y + 1),
			Point(p.x - 1, p.y) };
}

void pixel::neighbor(const Mat& mask, int gradType) {

	if (gradType == GRAD_IMPORTING) {
		for (auto n : nn) {
			if (n.x >= 0 && n.y >= 0 && n.x < mask.cols && n.y < mask.rows) {
				if (mask.at<uchar>(n.y, n.x) == 0)	b_Np.push_back(n);
				else								in_Np.push_back(n);
			}
		}
	}
	else if (gradType == GRAD_MIXING) {
		for (auto n : nn) {
			if (n.x > 0 && n.y > 0 && n.x < mask.cols-1 && n.y < mask.rows-1) {
				if (mask.at<uchar>(n.y, n.x) == 0) 	b_Np.push_back(n);
				else								in_Np.push_back(n);
			}
		}
	}
}

void pixel::gradient(const Mat& src, const Mat& dst, int flag) {

	if (flag == GRAD_IMPORTING) {
		for (Point n : in_Np)	grad += (src.at<Vec3f>(p.y, p.x) - src.at<Vec3f>(n.y, n.x));
		for (Point n : b_Np)	grad += (src.at<Vec3f>(p.y, p.x) - src.at<Vec3f>(n.y, n.x));
	}

	else if (flag == GRAD_MIXING) {
		for (Point n : in_Np) 	grad += gradMax(p, n, src, dst);
		for (Point n : b_Np)	grad += gradMax(p, n, src, dst);
	}
}

Vec3f pixel::gradMax(const Point p, const Point n, const Mat& src, const Mat& dst) {
	auto f = (dst.at<Vec3f>(p.y, p.x) - dst.at<Vec3f>(n.y, n.x));
	auto g = (src.at<Vec3f>(p.y, p.x) - src.at<Vec3f>(n.y, n.x));

	return f.dot(f) > g.dot(g) ? f : g;
}

void pixel::new_pixel(const Mat& src, Mat& dst, const Mat& img) {
	Vec3f fq(0, 0, 0);

	for (Point n : in_Np)		fq += src.at<Vec3f>(n.y, n.x);
	for (Point n : b_Np )		fq += img.at<Vec3f>(n.y, n.x);

	dst.at<Vec3f>(p.y, p.x) = (fq + grad) / (float)(in_Np.size() + b_Np.size());
}