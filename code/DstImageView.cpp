#include "DstImageView.hpp"

bool DstImageView::handle(int e) {
	if (e == JGL::EVENT_PUSH && _JGL::eventButton() == JGL::EVENT_LBUTTON) {
		oldP = screenToImage(_JGL::eventPt());
		return true;
	}
	else if (e == JGL::EVENT_DRAG && _JGL::eventButton() == JGL::EVENT_LBUTTON) {
		if (hoverPoint < 0) {
			vec2 p = _JGL::eventPt();
			vec2 deltaP = screenToImage(p) - oldP;
			dx += deltaP.x;
			dy += deltaP.y;
			
			vec2 dst_br = (vec2(dst.cols, dst.rows));
			vec2 src_br = (vec2(src.cols, src.rows));
			
			oldP = screenToImage(p);
			redraw();
		}
		else {
			vec2 p = screenToImage(_JGL::eventPt());
			vec2 offset = vec2(dx + src.cols * scaleX, dy + src.rows * scaleY);
			switch (hoverPoint) {
			case tl: {
				scaleX = (offset.x - p.x) / src.cols;
				scaleY = (offset.y - p.y) / src.rows;
				dx = p.x;
				dy = p.y;
			} break;
			case tr: {
				scaleX = (p.x - dx) / src.cols;
				scaleY = (offset.y - p.y) / src.rows;
				dy = p.y;
			} break;
			case bl: {
				scaleX = (offset.x - p.x) / src.cols;
				scaleY = (p.y - dy) / src.rows;
				dx = p.x;
			} break;
			case br: {
				scaleX = (p.x - dx) / src.cols;
				scaleY = (p.y - dy) / src.rows;
			} break;
			default:
				break;
			}
			redraw();
			return true;
		}
	}

	// hover event handler
	else if (e == JGL::EVENT_MOVE) {
		if (src.cols > 0 || src.rows > 0) // make mask btn을 통해서 src 값이 들어오면 동작
		{
			vec2 p = _JGL::eventPt(); // Screen Point
			vector<vec2> corners = { imageToScreen(vec2(dx, dy)),
									imageToScreen(vec2(dx + src.cols * scaleX, dy)),
									imageToScreen(vec2(dx, dy + src.rows * scaleY)),
									imageToScreen(vec2(dx + src.cols * scaleX, dy + src.rows * scaleY)) };

			int newHoverPoint = -1;

			for (auto i = 0; i < 4; i++) {
				if (length(corners[i] - p) < 4) // 근처가면 if문 -> 4를 조절해서 hover 범위를 조절할 수 있음
				{
					newHoverPoint = i; // hover point index update
					break;
				}
			}

			if (hoverPoint != newHoverPoint) // 현재 위치가 Dot에 있다면 
			{
				hoverPoint = newHoverPoint;
				if (hoverPoint < 0)
					_JGL::setCursor(GLFW_ARROW_CURSOR);
				else
					_JGL::setCursor(GLFW_HAND_CURSOR);
				redraw();
				return true;
			}
		}
	}
	else if (e == JGL::EVENT_DND) {
		dst = imread(_JGL::eventDND()[0]);
		clearImageView();
		setImage(dst.data, dst.cols, dst.rows, 3);
	}
	return ImageViewer::handle(e);
}

void DstImageView::drawGL(){
	ImageViewer::drawGL();
	// x, y축 방향 scale - scaleX, scaleY
	// offset point(top left point) - dx, dy
	mat3 mat = mat3(scaleX, 0, 0, 0, scaleY, 0, dx, dy, 1); // homogeneous coord(scale + translation)
	_srcImg.draw(viewProjection(), mat);
}

void DstImageView::drawContents(NVGcontext* vg, const rect& r, int a) {
	// Draw dots and rectangle in texImg
	if (_srcImg._imageHeight > 0) {
		vector<vec2> corners = { imageToScreen(vec2(dx, dy)),
								imageToScreen(vec2(dx + src.cols * scaleX, dy)),
								imageToScreen(vec2(dx, dy + src.rows * scaleY)),
								imageToScreen(vec2(dx + src.cols * scaleX, dy + src.rows * scaleY)) };
		drawRect(vg, glm::rect(corners[0], corners[3]), 1, nvgRGBAf(1, 0, 0, 1));
		for (auto c : corners)
			drawDot(vg, c, 2, nvgRGBAf(1, 1, 1, 1));
		if (hoverPoint >= 0)
			drawDot(vg, corners[hoverPoint], 3, nvgRGBAf(1, .7, 0, 1));
	}
}

void DstImageView::setMask(Mat mask) {
	src = Mat();
	msk = Mat();

	dx = 0;
	dy = 0;
	scaleX = 1.f;
	scaleY = 1.f;

	vector<Mat> channels(4);
	split(mask, channels);
	Mat srcChannel[3] = { channels[0] , channels[1] , channels[2] };

	msk = channels[3];
	merge(srcChannel, 3, src);
	Mat tmp = Mat(src.size(), CV_32FC4);
	mask.copyTo(tmp, msk);

	_srcImg.setImage(tmp.data, tmp.cols, tmp.rows, 4);
}

void DstImageView::clearImageView() {
	setImage(dst.data, dst.cols, dst.rows, 3);
	results.clear();
	results.push_back(dst);
	_srcImg.clear();
	src = Mat();
	msk = Mat();
	
	dx = 0;
	dy = 0;
	scaleX = 1.f;
	scaleY = 1.f;
}

void DstImageView::undo() {
	if (results.size() == 1)
		setImage(dst.data, dst.cols, dst.rows, 3);
	else if (results.size() > 1) {
		Mat lastres;
		lastres = results.back().clone();
		results.pop_back();
		_srcImg.clear();
		setImage(results.back().data, results.back().cols, results.back().rows, 3);
	}
}

void DstImageView::compose(int gradType) {

	if (src.empty()) return;

	// src, msk scale
	cv::resize(src, src, Size(src.cols * scaleX, src.rows * scaleY), 1, 1, 0);
	cv::resize(msk, msk, Size(msk.cols * scaleX, msk.rows * scaleY), 1, 1, 0);
	
	// 자를 rect 범위 만들기
	float x0, x1, y0, y1, w0, w, h0, h;
	w0 = dst.cols;
	h0 = dst.rows;
	w = src.cols;
	h = src.rows;
	
	x0 = std::max(dx, 0.f);
	y0 = std::max(dy, 0.f);
	x1 = std::min(dx + w, w0);
	y1 = std::min(dy + h, h0);

	Rect dstRoi(x0, y0, x1 - x0, y1 - y0);
	Rect srcRoi(x0 - dx, y0 - dy, x1 - x0, y1 - y0);

	// cropping src, msk, dst;
	Mat srcCropped(srcRoi.size(), CV_8U), mskCropped(srcRoi.size(), CV_8UC1), dstCropped(dstRoi.size(), CV_8U);
	src(srcRoi).copyTo(srcCropped);
	msk(srcRoi).copyTo(mskCropped);
	Mat res = results.back().clone();
	res(dstRoi).copyTo(dstCropped);

	Poisson poisson(srcCropped, dstCropped, mskCropped);
	poisson.makeTable(gradType);
	Mat fin = poisson.run();
	fin.convertTo(fin, CV_8U, 255);
	fin.copyTo(res(dstRoi));
	results.push_back(res);
	setImage(res.data, res.cols, res.rows, 3);

	_srcImg.clear();
	src = Mat();
	msk = Mat();
}