#include "SimpleImage.h"
#include "SimpleVideo.h"
#include <math.h>
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char** argv) { 
  if(argc < 3) {
    cerr << "usage: ./combiner <base filename> <total number of frames> "
        << "[<video name> <fps>]" << endl;
    exit(1);
  } else if(argc > 5) {
    cout << "Ignoring excessive arguments..." << endl;
  } 

  string videoName = "combined";
  string baseFilename(argv[1]);
  int numFrames(atoi(argv[2]));
  double fps = 20.0;

  int width = ceil(log10(numFrames)); // for formatting the string
  if (argc > 3) {
    videoName = argv[3];
  }

  if (argc > 4) {
    fps = (atof(argv[4]));
  }

  videoName += ".avi";
  
  // Collect all the frames
  vector<unique_ptr<SimpleImage> > images;
  
  for (int i = 1; i <= numFrames; i++) {
    stringstream count;
	count << setw(width) << setfill('0') << i;
	stringstream ss;
	ss << baseFilename << count.str() << ".png";
	unique_ptr<SimpleImage> image(new SimpleImage(ss.str()));
	images.push_back(move(image));
  }
  
  // Throw them together
  SimpleVideoWriter writer(videoName, fps, images[0]->GetWidth(),
      images[0]->GetHeight());
  writer.WriteToVideo(images);
}