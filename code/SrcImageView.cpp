#include "SrcImageView.hpp"

bool SrcImageView::handle(int e){
	if (e == JGL::EVENT_PUSH && _JGL::eventButton() == JGL::EVENT_LBUTTON) {
		boxControl = false;
		maskDone = true;
		pts.clear();
		return true;
	}
	else if (e == JGL::EVENT_DRAG && _JGL::eventButton() == JGL::EVENT_LBUTTON) {
		if (!src.empty() && maskDone) {
			vec2 p = _JGL::eventPt();
			p = screenToImage(p);
			if (p.x > 0 && p.y > 0 && p.x < src.cols && p.y < src.rows) {
				pts.push_back(Point(p.x, p.y));
				redraw();
				return true;
			}
		}
	}
	else if (e == JGL::EVENT_RELEASE && _JGL::eventButton() == JGL::EVENT_LBUTTON) {
		if (pts.empty()) return true;
		contours.push_back(pts);
		redraw();
		auto [minx, maxx] = minmax_element(pts.begin(), pts.end(), [](const auto& a, const auto& b) {return a.x < b.x; });
		auto [miny, maxy] = minmax_element(pts.begin(), pts.end(), [](const auto& a, const auto& b) {return a.y < b.y; });
		box.p = vec2((*minx).x > 0 ? (*minx).x - 1 : 0, (*miny).y > 0 ? (*miny).y - 1 : 0);
		box.q = vec2((*maxx).x < src.cols - 1 ? (*maxx).x + 1 : src.cols - 1, (*maxy).y < src.rows - 1 ? (*maxy).y + 1 : src.rows - 1);
		boxControl = true;
		maskDone = false;
		return true;
	}
	else if (e == JGL::EVENT_DND) {
		src = imread(_JGL::eventDND()[0]);
		clearImageView();
	}
	return ImageViewer::handle(e);
}

void SrcImageView::drawContents(NVGcontext* vg, const rect& r, int align) {
	ImageViewer::drawContents(vg, r, align);
	if (!pts.empty())
		drawContourLine(vg, pts, nvgRGBAf(0, 0, 0, 0.7), nvgRGBAf(0, 1, 0, 1));
	for (auto& c : contours)
		drawContourLine(vg, c, nvgRGBAf(0, 0, 0, 0.7), nvgRGBAf(1, 0.5, 0, 1));

	if (boxControl)
		drawRect(vg, getScreenRect(rect(box.p, box.q)), 1, nvgRGBAf(1, 0, 0, 1));
}

void SrcImageView::drawContourLine(NVGcontext* vg, vector<Point> points, NVGcolor bc, NVGcolor fc) {
	nvgBeginPath(vg);
	vec2 p = imageToScreen(vec2(points[0].x, points[0].y));
	nvgMoveTo(vg, p.x, p.y);
	for (auto i = 1; i < points.size(); i++) {
		vec2 p = imageToScreen(vec2(points[i].x, points[i].y));
		nvgLineTo(vg, p.x, p.y);
	}
	nvgStrokeColor(vg, nvgRGBAf(bc.r, bc.g, bc.b, bc.a));
	nvgStrokeWidth(vg, 3);
	nvgStroke(vg);
	nvgStrokeColor(vg, nvgRGBAf(fc.r, fc.g, fc.b, fc.a));
	nvgStrokeWidth(vg, 1);
	nvgStroke(vg);
}

void SrcImageView::clearImageView(void) {
	setImage(src.data, src.cols, src.rows, 3);
	contours.clear();
	pts.clear();
	box.p = vec2(0, 0);
	box.q = vec2(0, 0);
	maskDone = false;
}

Mat SrcImageView::getMask(void) {
	if (!src.empty()) {
		Rect roi(Point(box.p[0], box.p[1]), Point(box.q[0], box.q[1]));

		Mat cropped_rgb(Size(roi.width, roi.height), CV_8UC3);
		Mat cropped_msk(Size(roi.width, roi.height), CV_8UC1);

		Mat res_msk;

		if (contours.size() > 0)
		{
			msk = Mat(src.size(), CV_8UC1, Scalar(0));
			drawContours(msk, contours, -1, Scalar(255), -1);
		}

		src(roi).copyTo(cropped_rgb);
		msk(roi).copyTo(cropped_msk);

		vector<Mat> tmp(4);
		split(cropped_rgb, tmp);
		tmp.push_back(cropped_msk);
		merge(tmp, res_msk);

		maskDone = false;
		clearImageView();

		return res_msk;
	}
	return Mat();
}