#include <JGL/JGL.hpp>
#include <JGL/JGL_Window.hpp>
#include <JGL/JGL_Widget.hpp>
#include <JGL/JGL_Toolbar.hpp>
#include <JGL/JGL_Aligner.hpp>
#include <JGL/JGL_Options.hpp>
#include <JGL/JGL_Button.hpp>
#include <JGL/JGL_Splitter.hpp>

#include "SrcImageView.hpp"
#include "DstImageView.hpp"

using namespace JGL;

int gradType;
SrcImageView* img_v;							// ImageViewer of the Source part
DstImageView* img_v2;							// ImageViewer of the destination part

void gradTypeCallback(Widget* w, void* ud);		// get gradient Type from Options
void saveBtnCallback(Widget* w, void* ud);		// save the result image
void maskBtnCallback(Widget* w, void* ud);		// make the mask in img_v and pass to img_v2
void showBtnCallback(Widget* w, void* ud);		// show the img_v2 result by applying Poisson method
void undoBtnCallback(Widget* w, void* ud);		// Undo one step
void clearSBtnCallback(Widget* w, void* ud);	// restore the original src image
void clearDBtnCallback(Widget* w, void* ud);	// restore the original dst image

int main(int argc, const char* argv[]) {
	Window* window = new Window(1240, 600, "Poisson Image Editing");

	window->alignment(JGL::ALIGN_ALL);

	Aligner* aligner = new Aligner(0, 0, window->w(), window->h());
	aligner->type(Aligner::VERTICAL);
	aligner->alignment(ALIGN_ALL);

		Toolbar* toolbar = new Toolbar(0, 0, window->w(), _size_toolbar_height());
		Options* type = new Options(0, 0, 300, _size_button_height());
		type->add("Importing gradient");
		type->add("Mixing gradient");
		type->value(gradType);
		type->callback(gradTypeCallback);
		Button* saveBtn = new Button(0, 0, 200, "Save");
		saveBtn->callback(saveBtnCallback);
		toolbar->end();

		Splitter* split_1 = new Splitter(0, 0, aligner->w(), 10);
		split_1->type(Aligner::HORIZONTAL);

			Aligner* aligner1 = new Aligner(0, 0, window->w() / 2, window->h());
			aligner1->type(Aligner::VERTICAL);
			aligner1->alignment(ALIGN_ALL);
				Toolbar* t1 = new Toolbar(0, 0, 10, _size_toolbar_height(), "src_toolbar");
				t1->alignment(ALIGN_ALL);
				Button* maskBtn = new Button(0, 0, 100, "Make contour");
				Button* clearSBtn = new Button(0, 0, 100, "Clear image");
				maskBtn->callback(maskBtnCallback);
				clearSBtn->callback(clearSBtnCallback);
				t1->end();

				Mat init = imread("./img/background.png");
				putText(init, "Input a source image.", Point(init.cols * 0.28, init.rows * 0.5), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0));
				img_v = new SrcImageView(0, 0, split_1->w() / 2, init.rows);
				img_v->setImage(init.data, init.cols, init.rows, 3);
			aligner1->resizable(img_v);
			aligner1->end();

			Aligner* aligner2 = new Aligner(0, 0, window->w() / 2, window->h());
			aligner2->type(Aligner::VERTICAL);
			aligner2->alignment(ALIGN_ALL);

				Toolbar* t2 = new Toolbar(0, 0, 120, _size_toolbar_height(), "dst_toolbar");
				t2->alignment(ALIGN_ALL);
				Button* showBtn = new Button(10, 0, 100, "Show result");
				Button* clearDBtn = new Button(0, 0, 100, "Clear image");
				Button* undoBtn = new Button(0, 0, 100, "Undo");
				showBtn->callback(showBtnCallback);
				clearDBtn->callback(clearDBtnCallback);
				undoBtn->callback(undoBtnCallback);
				t2->end();

				init = imread("./img/background.png");
				putText(init, "Input a destination image.", Point(init.cols * 0.25, init.rows * 0.5), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0));
				img_v2 = new DstImageView(0, 0, split_1->w() / 2, init.rows);
				img_v2->setImage(init.data, init.cols, init.rows, 3);
			aligner2->resizable(img_v2);
			aligner2->end();

		split_1->resizable(aligner1);
		split_1->end();

	aligner->resizable(split_1);
	aligner->end();

	window->show();
	_JGL::run();
	return 0;
}
//int currentgrad = 0;

void gradTypeCallback(Widget* w, void* ud) {
	gradType = ((Options*)w)->value();
	img_v->redraw();
	//img_v2->clearImageView();
}

void saveBtnCallback(Widget* w, void* ud) {
	cv::imwrite("./img/Final_result.png", img_v2->getres());
}

void maskBtnCallback(Widget* w, void* ud) {
	auto result = img_v->getMask();

	if (result.empty()) return;
	else img_v2->setMask(result);
}

void clearSBtnCallback(Widget* w, void* ud) {
	img_v->clearImageView();
}

void clearDBtnCallback(Widget* w, void* ud) {
	img_v2->clearImageView();
}

void showBtnCallback(Widget* w, void* ud) {
	img_v2->compose(gradType);
}

void undoBtnCallback(Widget* w, void* ud) {
	img_v2->undo();
}