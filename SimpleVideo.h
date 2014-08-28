#ifndef _SIMPLEVIDEO_H_
#define _SIMPLEVIDEO_H_ 

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <memory>

#include "SimpleImage.h"

using namespace cv;
using namespace std;

class SimpleVideoReader {
public:
  SimpleVideoReader(const string& filename) : delegate_(VideoCapture(filename)) {}

  bool IsOpened() { return delegate_.isOpened(); }

  vector<SimpleImage> ExtractFrames(int frames) {
	vector<SimpleImage> ret;
    Mat frame;
	for (int i = 0; i < frames; i++) {
	  delegate_ >> frame;
	  if (frame.empty()) {
		return ret;
	  }
	  SimpleImage image;
	  image.Initialize(frame.cols, frame.rows); 
	  for (int x = 0; x < frame.cols; x++) {
		for (int y = 0; y < frame.rows; y++) {
		  Vec3b pixel = frame.at<Vec3b>(y, x);
		  uchar blue = pixel.val[0];
		  uchar green = pixel.val[1];
		  uchar red = pixel.val[2];
		  image.setPixel(x, frame.rows - y - 1,
		      RGBColor(red/255.f, green/255.f, blue/255.f));
		}
	  }
	  ret.push_back(image);
	}
	return ret;
  }
private:
  VideoCapture delegate_;
};

class SimpleVideoWriter {
public:
  SimpleVideoWriter(const string& filename, double fps, int width, int height) {
    delegate_ = VideoWriter(filename, CV_FOURCC('D', 'I', 'V', 'X'),
        fps, Size(width, height), true);
  }
  void WriteToVideo(const vector<unique_ptr<SimpleImage> >& images) {
    for (auto image = images.begin();
		image != images.end();
		++image) {
	  Size thisSize = Size((*image)->GetWidth(), (*image)->GetHeight());
	  Mat frame(Size((*image)->GetWidth(), (*image)->GetHeight()), CV_8UC3);
	  for (int x = 0; x < (*image)->GetWidth(); x++) {
  	    for (int y = 0; y < (*image)->GetHeight(); y++) {
		  RGBColor rgb = (**image)(x, (*image)->GetHeight() - y - 1);
		  Vec3b bgr(rgb.b * 255.f, rgb.g * 255.f, rgb.r * 255.f);
		  frame.at<Vec3b>(y, x) = bgr;
		}
	  }
	  delegate_ << frame;
	}
  }
private:
  VideoWriter delegate_;
};

#endif