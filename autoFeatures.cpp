/**
 * Creates features using Haar Cascades facial detection.
 * Based on http://docs.opencv.org/doc/tutorials/objdetect/cascade_classifier/cascade_classifier.html
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <fstream>

#include "util.h"

using namespace cv;
using namespace std;

void detectAndDisplay(Mat frame, string outName);

 /** Global variables */
String nose_cascade_name = "haars/haarcascade_mcs_nose.xml";
CascadeClassifier nose_cascade;
String eye_cascade_name = "haars/haarcascade_eye.xml";
CascadeClassifier eye_cascade;
String mouth_cascade_name = "haars/haarcascade_mcs_mouth.xml";
CascadeClassifier mouth_cascade;
string window_name = "Capture - Face detection";

int main(int argc, const char** argv) {
  if(argc < 3) {
    cerr << "usage: ./autoFeatures <input.png> <featurebasename.feat>"
         << endl;
    exit(1);
  } else if(argc > 3) {
    cout << "Ignoring extra arguments..." << endl;
  } 
  CvCapture* capture;
  Mat frame;
  
  //-- 1. Load the cascades
  if(!nose_cascade.load(nose_cascade_name)) {
    printf("--(!)Error loading nose xml\n");
	  return -1;
  }
  if(!eye_cascade.load(eye_cascade_name)){
    printf("--(!)Error loading eye xml\n");
	  return -1;
  }
  if(!mouth_cascade.load(mouth_cascade_name)){
    printf("--(!)Error loading mouth xml\n");
	  return -1;
  }

  Mat image = imread(string(argv[1]), IMREAD_COLOR); // Read the file
  detectAndDisplay(image, string(argv[2]));
 }

// Converts a rectangle to a pair of features:
// Top left --> top right and top left --> lower left
void RectToFeatures(Rect rect, int width, int height, vector<Feature>& features) {
  Point tl = rect.tl();
  Point bottomRight = rect.br();
  Vertex2f topLeft(rect.tl().x/float(width), rect.tl().y/float(height));
  Vertex2f bottomLeft(rect.tl().x/float(width), rect.br().y/float(height));
  Vertex2f topRight(rect.br().x/float(width), rect.tl().y/float(height));
  features.push_back(Feature(topLeft, bottomLeft));
  features.push_back(Feature(topLeft, topRight));
}

/** @function detectAndDisplay */
void detectAndDisplay(Mat frame, string outFile) {
  std::vector<Rect> faces;
  Mat frame_gray;

  cvtColor(frame, frame_gray, CV_BGR2GRAY);
  equalizeHist(frame_gray, frame_gray);

  vector<Feature> features;
  //-- Detect facial features
  nose_cascade.detectMultiScale(frame_gray, faces, 1.1, 2,
      0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
  for (int i = 0; i < std::min( (size_t) 1U, (size_t) faces.size()); i++) {
	  RectToFeatures(faces[i], frame.cols, frame.rows, features);
    rectangle(frame, faces[i], Scalar(255, 0, 0));
  }  
  eye_cascade.detectMultiScale(frame_gray, faces, 1.1, 2,
      0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
  for (int i = 0; i < std::min( (size_t) 2U, (size_t) faces.size()); i++) {
	  RectToFeatures(faces[i], frame.cols, frame.rows, features);
    rectangle(frame, faces[i], Scalar(255, 0, 0));
  } 
  mouth_cascade.detectMultiScale(frame_gray, faces, 1.1, 2,
      0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
  for (int i = 0; i < std::min( (size_t) 1U, (size_t) faces.size()); i++) {
    RectToFeatures(faces[i], frame.cols, frame.rows, features);
	  rectangle(frame, faces[i], Scalar(255, 0, 0));
  }

  // Write out the features
  ofstream outStream(outFile);
  for (auto feature = features.begin(); feature != features.end(); ++feature) {
    outStream << *feature;
  }
  outStream.close();

  // Show what we got
  imshow(window_name, frame);
  waitKey(0);
 }