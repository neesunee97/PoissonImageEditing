#ifndef SrcImageView_h
#define SrcImageView_h

#include <opencv2/opencv.hpp>
#include <JGL/JGL_ImageViewer.hpp>

using namespace cv;
using namespace JGL;

struct SrcImageView : public ImageViewer {

	vector<Point> pts;
	vector<vector<Point>> contours;
	Mat src, msk;

	bool boxControl = false;
	bool maskDone = false;

	struct boundingBox{
		vec2 p = vec2(0, 0);
		vec2 q = vec2(0, 0);
	} box;

	SrcImageView(float x, float y, float w, float h, const string& s = "") : ImageViewer(x, y, w, h, s) {}

	virtual bool handle(int e) override;
	void drawContents(NVGcontext* vg, const rect& r, int align) override;

	void drawContourLine(NVGcontext* vg, vector<Point> points, NVGcolor bc, NVGcolor fc);

	/* button callback */
	void clearImageView(void);
	Mat getMask(void);
};

#endif // !SrcImageView_h

