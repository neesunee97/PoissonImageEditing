#ifndef DstImageView_h
#define DstImageView_h

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <JGL/JGL_ImageViewer.hpp>

#include "NPoisson.hpp"

using namespace cv;
using namespace JGL;

struct DstImageView : public ImageViewer {

	const enum rectPoint{	tl, tr, bl,	br,	};

	vec2 oldP = vec2(0, 0);
	
	float dx = 0;
	float dy = 0;
	float scaleX = 1.f;
	float scaleY = 1.f;
	int hoverPoint = -1;

	Mat src, dst, msk;
	vector<Mat> results;
	TexImage _srcImg;

	DstImageView(float x, float y, float w, float h, const string& s = "") : ImageViewer(x, y, w, h, s) {}
	
	Mat getres(void) { return results.empty() ? Mat() : results.back(); }

	virtual bool handle(int e) override;
	virtual void drawGL(void) override;
	virtual void drawContents(NVGcontext* vg, const rect& r, int a) override;
	
	void setMask(Mat mask);
	
	/* button callback */
	void undo(void);
	void clearImageView(void);
	void compose(int gradType);
};

#endif // !DstImageView_h

