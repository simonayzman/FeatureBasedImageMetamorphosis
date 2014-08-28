#include "SimpleImage.h"
#include "SimpleVideo.h"
#include "util.h"
#include "findGL.h"
#include "findGLUT.h"

#include <fstream>

RGBColor WHITE = RGBColor(1.0, 1.0, 1.0);
int windowHeight;
int windowWidth;

//parameters for weight - can be adjusted
const float a = 0.5f;
const float b = 1.f; 
const float p = 0.2f;

using namespace std;

typedef unique_ptr<SimpleImage> ImagePtr;
typedef float (*EasingFunction)(float);

string intermediaryImagesDirectory = "intermediates/";
string intermediaryImagesBaseName = "intermediate";

vector<Feature> getFeatures(string filename) {
  int len = filename.length();
  if(len < 5) {
    cerr << "Invalid file format. A .feat file is required." << endl;
    exit(1);
  } else if(filename.substr(len - 5, 5).compare(".feat") != 0){
    cerr << "Invalid file format. A .feat file is required." << endl;
    exit(1);        
  }    
  string line;
  ifstream myfile(filename);
  vector<Feature> f;
  if(myfile.is_open()) {
    while(getline(myfile, line)) {
      istringstream is(line);
      Vertex2f p, q;
      is >> p.x >> p.y >> q.x >> q.y;
      f.push_back(Feature(p*windowHeight, q*windowHeight));
    }
  } else {
    cerr << "Unable to open file." << endl;
    exit(1);          
  }
  return f;
}

bool in_bound(int x, int y){
  return (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight);
}

RGBColor clampedColor(const SimpleImage& image, int x, int y){
  return (in_bound(x, y)) ? image(x, y) : WHITE;
}

//bilinear interpolation of color according to corner vertices
RGBColor getColor(const SimpleImage& image, Vertex2f pixel) {
  double fx, dx, fy, dy;
  dx = modf(pixel.x, &fx);
  dy = modf(pixel.y, &fy);
  RGBColor downleft = clampedColor(image, fx, fy);
  RGBColor downright = clampedColor(image, fx+1, fy);
  RGBColor upleft = clampedColor(image, fx, fy+1);
  RGBColor upright = clampedColor(image, fx+1, fy+1);  
  RGBColor down = linColor(downleft, downright, dx);
  RGBColor up = linColor(upleft, upright, dx);
  return linColor(down, up, dy);
}

Vector2f Perpendicular(Vector2f v) {
  return Vector2f(v.y, -v.x);
}

float ShortestDistToFeature(Vertex2f P, Feature F, float u, float v){
  if(u >= 0 && u <= 1) return abs(v);
  if(u < 0) return Distance(P, F.p);
  return Distance(P, F.q);
}

float weightFunc(float length, float dist, float a, float b, float p){
  return pow(pow(length, p)/(a + dist), b);
}

//field morphing
ImagePtr FieldMorphFixedTime(const SimpleImage& image,
    vector<Feature> sourceFeatures, vector<Feature> destinationFeatures) {
  int numFeatures = sourceFeatures.size();
  ImagePtr result(new SimpleImage(windowWidth, windowHeight, WHITE));
  for(int x = 0; x < windowWidth; ++x) {
    for(int y = 0; y < windowHeight; ++y) { 
	  //For each pixel X in the destination
      Vertex2f DSUM = Vertex2f(0.f, 0.f);
      float weightsum = 0.f;
      Vertex2f destinationPixel = Vertex2f((float)x, (float)y); //X
      for(int i = 0; i < numFeatures; ++i) { //for each feature (Pi, Qi)
        Feature SF = sourceFeatures[i];
        Feature DF = destinationFeatures[i];
        Vector2f sourceBase = FeatureVec(SF);//Q' - P'
        float sourceBaseLength = length(SF);//||Q' - P'||                
        Vector2f destinationBase = FeatureVec(DF); //Q - P
        float destinationBaseLength = length(DF); //||Q - P||
        Vector2f destinationPerp = Perpendicular(destinationBase); //Perpendicular(Q-P)               
        Vector2f sourcePerp = Perpendicular(sourceBase); //Perpendicular(Q'-P')
        Vector2f destinationDisp = VertToVec(destinationPixel - DF.p); //X - P
        float u = dotProduct(destinationDisp, destinationBase)
			/ pow(destinationBaseLength, 2.f);
        float v = dotProduct(destinationDisp, destinationPerp) 
			/ destinationBaseLength;
        Vertex2f resultingPixel = SF.p + (sourceBase * u)
			+ (sourcePerp * v / sourceBaseLength);
        Vector2f displacement = VertToVec(resultingPixel - destinationPixel); 
        float dist = ShortestDistToFeature(destinationPixel, DF, u, v); //distance from X to feature PiQi
        float weight = weightFunc(destinationBaseLength, dist, a, b, p);
        DSUM = DSUM + displacement * weight;
        weightsum += weight;
      }
      Vertex2f sourcePixel = destinationPixel + DSUM/weightsum; //X'
      if(in_bound(sourcePixel.x, sourcePixel.y)){
        result->setPixel(x, y, getColor(image, sourcePixel));
      }
    }
  }
  return result;  
}

ImagePtr FieldMorph(const SimpleImage& image, vector<Feature> sourceFeatures,
    vector<Feature> destinationFeatures, float t) {
  vector<Feature> intermediateFeatures;
  int numFeatures = sourceFeatures.size();
  for(int i = 0; i < numFeatures; ++i) {
	intermediateFeatures.push_back(
		linFeature(sourceFeatures[i], destinationFeatures[i], t));
  }
  return FieldMorphFixedTime(image, sourceFeatures, intermediateFeatures);
}

/* Linear blend of the two images (pixel by pixel) of parameter t 
   (t from 0 to 1)*/
ImagePtr LinearBlend(SimpleImage *src, SimpleImage *dest, float t) {
  ImagePtr result(new SimpleImage(windowWidth, windowHeight, WHITE));
  for(int i = 0; i < windowWidth; ++i) {
    for(int j = 0; j < windowHeight; ++j) {
      result->setPixel(i, j, linColor((*src)(i, j), (*dest)(i, j), t));
    }
  }   
  return result;
}

ImagePtr ImageMorph(const SimpleImage& source, const SimpleImage& destination,
    vector<Feature> sourceFeatures, vector<Feature> destinationFeatures, float t) {
  ImagePtr sourceMorphed = FieldMorph(source, sourceFeatures, destinationFeatures, t); 
  ImagePtr destinationMorphed = FieldMorph(destination, destinationFeatures, sourceFeatures,
      1.f - t);
  return LinearBlend(sourceMorphed.get(), destinationMorphed.get(), t);
}

//generate the intermediate images
vector<ImagePtr> FramesMorph(const SimpleImage& sourceImage,
    const SimpleImage& destinationImage, string sourceFilename,
	string destinationFilename, int numFrames, EasingFunction easer) {
  vector<ImagePtr> ret;
  vector<Feature> sourceFeatures = getFeatures(sourceFilename);
  vector<Feature> destinationFeatures = getFeatures(destinationFilename);
  for(int i = 0; i < numFrames; ++i) {
    float t = easer(float(i)/float(numFrames));
    ImagePtr intermediateImage = ImageMorph(sourceImage, destinationImage,
		sourceFeatures, destinationFeatures, t);
    string intermediateFilename = intermediaryImagesDirectory 
		+ intermediaryImagesBaseName 
		+ (i < 10 ? "0" : "") 
		+ to_string(i) 
		+ ".png";
    intermediateImage->WritePNG(intermediateFilename);
	ret.push_back(move(intermediateImage));
  }
  return ret;
}

int main(int argc, char** argv) {
  if(argc < 8) {
    cerr << "usage: ./imageMorph <source.feat> <destination.feat> <source.png> "
	    << "<destination.png> <intermediaryImagesBaseName> <numFrames> "
		<< "<mode(-l or -s), -l indicates linear and -s indicates "
		<< "sine curve>" << endl;
    exit(1);
  } else if(argc > 8) {
    cout << "Ignoring excessive arguments..." << endl;
  } 
  SimpleImage sourceImage(argv[3]);
  SimpleImage destinationImage(argv[4]);
  intermediaryImagesBaseName = argv[5];
  string indicator = argv[7];
  EasingFunction easer;
  EasingFunction linear = [] (float fraction) -> float { return fraction; };
  EasingFunction sinusoidal = [] (float fraction) -> float {
      return 0.5*((sin(fraction - 0.5)*PI) + 1);
  };

  if (indicator.compare("-l") == 0) {
    easer = linear;
  } else if(indicator.compare("-s") == 0) {
    easer = sinusoidal;
  } else {
    cout << "Invalid mode selected. For now we use LINEAR mode as default."
        << endl;
    easer = linear;
  }	

  windowWidth = max(sourceImage.GetWidth(), destinationImage.GetWidth());
  windowHeight = max(sourceImage.GetHeight(), destinationImage.GetHeight());

  vector<ImagePtr> frames = FramesMorph(sourceImage, destinationImage, argv[1], 
      argv[2], atoi(argv[6]), easer); //argv[6] is the number of frames
  SimpleVideoWriter videoWriter("out.avi", 15, windowWidth, windowHeight);
  videoWriter.WriteToVideo(frames);
  return 0;
}