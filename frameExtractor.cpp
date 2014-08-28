#include "SimpleImage.h"
#include "SimpleVideo.h"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
  if(argc < 4) {
    cerr << "usage: ./frameExtractor <input.avi> <numFrames> <basePngName>" << endl;
    exit(1);
  } else if(argc > 4) {
    cout << "Ignoring extra arguments..." << endl;
  } 
  std::string videoFile(argv[1]);
  SimpleVideoReader reader(videoFile);
  if (!reader.IsOpened()) {
    cout << "Could not open or find the video" << std::endl;
    return -1;
  }
  vector<SimpleImage> images = reader.ExtractFrames(atoi(argv[2]));
  for (int i = 0; i < images.size(); i++) {
    std::string tostr = to_string(i);
    std::string filename = string(argv[3]) + to_string(i) + ".png";
    images[i].WritePNG(filename);
  }
  return 0;
}