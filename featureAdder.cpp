#include <GLUT/glut.h>

#include <iostream>
#include <vector>
#include <stack>
#include <fstream>

#include "util.h"
#include "SimpleImage.h"

void KeyCallback(unsigned char key, int x, int y);
void MouseCallback(int button, int state, int x, int y);

void uploadFeatureFile(string FeatureFile, string type);
void outputFeatureFiles(bool withOverride = false);
void staggerUploadedPreviousActions();
void undoLastAction();
void screenshot();
void cleanUp();

void parseArguments(int argc, char** argv);

// Features of images
vector<Feature> sourceImageFeatures;
vector<Feature> destinationImageFeatures;

// Last image to receive an action
enum PreviousActionTurn {SOURCE_IMAGE, DESTINATION_IMAGE};
stack<PreviousActionTurn> previousActionTurns;

// Type of action that last action was
enum PreviousAction {SOURCE_VERTEX_STACK_ADDED, DESTINATION_VERTEX_STACK_ADDED,
    SOURCE_FEATURE_ADDED, DESTINATION_FEATURE_ADDED};
stack<PreviousAction> previousSourceImageActions;
stack<PreviousAction> previousDestinationImageActions;

// Tracker of unfinished vertices
Vertex2f sourceImageSelectedVertex;
Vertex2f destinationImageSelectedVertex;
bool isVertexSelectedInSourceImage = false;
bool isVertexSelectedInDestinationImage = false;

// Input images
string sourceImageFileName;
string destinationImageFileName;

// Feature files to open/edit
string sourceImageFeaturesFileName = "";
string destinationImageFeaturesFileName = "";

// Output feature file information
string outputFilesSubDirectory = "features/";
string sourceImageFeaturesOutputFileName = "sourceImageFeatures";
string destinationImageFeaturesOutputFileName = "destinationImageFeatures";
int currentOutputFileCount = 1;

// Screenshots information
string screenshotSubDirectory = "screenshots/";
string screenshotFileName = "featureAdder";
int currentScreenshotCount = 1;

// Texture data
GLuint *imageTextures;
unsigned int sourceImageTexture;
unsigned int destinationImageTexture;

// Mouse information
int mouseButton = -1;

void DrawSeparator() {
  glColor3f(0.f, 0.f, 0.f);
  glBegin(GL_LINES);
    glVertex2f(0.f,-1.f);
    glVertex2f(0.f, 1.f);
  glEnd();	
}

void DrawSourceImage() {
  glBindTexture(GL_TEXTURE_2D, imageTextures[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
  SimpleImage sourceImage(sourceImageFileName);
  int sourceImageWidth = sourceImage.GetWidth();
  int sourceImageHeight = sourceImage.GetHeight();
	
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sourceImageWidth, sourceImageHeight,
      0, GL_RGB, GL_FLOAT, sourceImage.GetData());
	
  glColor3f(1.f, 1.f, 1.f);
  glBegin(GL_QUADS);
    glTexCoord2f(0.f,0.f);
    glVertex2f(-1.f,0.f);
		
    glTexCoord2f(1.f,0.f);
    glVertex2f(0.f,0.f);
		
    glTexCoord2f(1.f,1.f);
    glVertex2f(0.f,1.f);
		
    glTexCoord2f(0.f,1.f);
    glVertex2f(-1.f,1.f);
  glEnd();
}

void DrawDestinationImage() {
  glBindTexture(GL_TEXTURE_2D, imageTextures[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
  SimpleImage destinationImage(destinationImageFileName);
  int destinationImageWidth = destinationImage.GetWidth();
  int destinationImageHeight = destinationImage.GetHeight();
	
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, destinationImageWidth,
      destinationImageHeight, 0, GL_RGB, GL_FLOAT,
      destinationImage.GetData());	
	
  glColor3f(1.f, 1.f, 1.f);
  glBegin(GL_QUADS);
    glTexCoord2f(0.f,0.f);
    glVertex2f(0.f,0.f);
		
    glTexCoord2f(1.f,0.f);
    glVertex2f(1.f,0.f);
		
    glTexCoord2f(1.f,1.f);
    glVertex2f(1.f,1.f);
		
    glTexCoord2f(0.f,1.f);
    glVertex2f(0.f,1.f);
  glEnd();
}

void DrawFeatures(string image, string featurePortion) {
  vector<Feature> *featuresOrigin;
  if (image == "source") {
    featuresOrigin = &sourceImageFeatures;
  } else if (image == "destination") {
    featuresOrigin = &destinationImageFeatures;
  } else {
    cout << "Fatal error.\n"; 
    exit(0);
  }

  if (featurePortion == "points") {
    glPointSize(4.f); 
	glBegin(GL_POINTS);
  } else if (featurePortion == "lines") {
    glLineWidth(2.f); 
    glBegin(GL_LINES);
  } else {
    cout << "Fatal error.\n"; 
    exit(0);
  }

  for (int i = 0; i < featuresOrigin->size(); ++i) {
    Feature currentFeature = featuresOrigin->at(i);

	if (i == featuresOrigin->size() - 1) { // last created one
      if (featurePortion == "points") {
        glColor3f(0.f,1.f,0.f); // green start point
        glVertex2f(currentFeature.p.x,currentFeature.p.y);
        glColor3f(0.f,0.f,1.f); // blue end point
        glVertex2f(currentFeature.q.x,currentFeature.q.y);
      } else if (featurePortion == "lines") {
        glColor3f(1.f,0.f,0.f); // red line
        glVertex2f(currentFeature.p.x,currentFeature.p.y);
        glVertex2f(currentFeature.q.x,currentFeature.q.y);
      }
	} else {
      if (featurePortion == "points") {
	    glColor3f(0.f,1.f,0.f); // green start point
        glVertex2f(currentFeature.p.x,currentFeature.p.y);
        glColor3f(1.f,0.f,0.f); // red end point
        glVertex2f(currentFeature.q.x,currentFeature.q.y);
	  } else if (featurePortion == "lines") {
        glColor3f(0.f,0.f,1.f); // blue line
        glVertex2f(currentFeature.p.x,currentFeature.p.y);
        glVertex2f(currentFeature.q.x,currentFeature.q.y);
      }
	}
  }

  glEnd();
}
void DrawUnfinishedVertices(string image) {
  Vertex2f *selectedVertex;
  if (image == "source") {
    if (isVertexSelectedInSourceImage) {
      selectedVertex = &sourceImageSelectedVertex;
    } else {
      return;
    }
  } else if (image == "destination") {
    if (isVertexSelectedInDestinationImage) {
      selectedVertex = &destinationImageSelectedVertex;
    } else {
      return;
    }
  } else {
    cout << "Fatal error.\n"; 
    exit(0);
  }

  glColor3f(1.f,0.f,0.f);
  glPointSize(4.f); 

  glBegin(GL_POINTS);
  glVertex2f(selectedVertex->x,selectedVertex->y);
  glEnd();
}


void Draw() {
  glTranslatef(0.f,0.f,-1.f);
  DrawSourceImage();
  DrawDestinationImage();
  DrawSeparator();
	
  DrawFeatures("source","points");
  DrawFeatures("destination","points");
  DrawFeatures("source","lines");
  DrawFeatures("destination","lines");

  DrawUnfinishedVertices("source");
  DrawUnfinishedVertices("destination");
}

void DisplayCallback() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  Draw();

  glFlush();
}

void ReshapeCallback(int w, int h) {
  glViewport(0,0,w,h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-1.05f, 1.05f, -0.05f, 1.05f, 0.5f,1.5f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

int main(int argc, char **argv) {
  srand(time(NULL));

  parseArguments(argc, argv);

  uploadFeatureFile(sourceImageFeaturesFileName,"source");
  uploadFeatureFile(destinationImageFeaturesFileName,"destination");
  staggerUploadedPreviousActions();

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowPosition(50,0);
  glutInitWindowSize(1100,550);
  glutCreateWindow("Feature Adder");

  glClearColor(1.0f,1.0f,1.0f,1.0f);

  glEnable(GL_TEXTURE_2D);
  imageTextures = new GLuint[2];
  imageTextures[0] = sourceImageTexture;
  imageTextures[1] = destinationImageTexture;
  glGenTextures(2, imageTextures);

  glutDisplayFunc(DisplayCallback);
  glutReshapeFunc(ReshapeCallback);
  glutIdleFunc(DisplayCallback);
	
  glutKeyboardFunc(KeyCallback);
  glutMouseFunc(MouseCallback);

  glutMainLoop();

  cleanUp();

  return 0;
}

void KeyCallback(unsigned char key, int x, int y) {
  switch(key) {
    case 's':
	  screenshot();
	  break;
    case 'u':
      undoLastAction();
      break;
    case 'o':
      outputFeatureFiles();
      break;
    case 'f':
      outputFeatureFiles(true);
      break;
    case 'q':
      cleanUp();
      exit(0);
      break;
    default:
      break;
  }
  glutPostRedisplay();
}

void MouseCallback(int button, int state, int x, int y) {
  if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev(GL_PROJECTION_MATRIX, projection );
    glGetIntegerv(GL_VIEWPORT, viewport );

    winX = (float) x;
    winY = (float) viewport[3] - (float) y;

    gluUnProject(winX, winY, 0, modelview, projection, viewport, &posX,
        &posY, &posZ);

    posX = max(-1., min(1.,posX));
    posY = max( 0., min(1.,posY));

    if (posX < 0.f) {// source image
      if (isVertexSelectedInSourceImage) {
        Feature newFeature;
        newFeature.p = sourceImageSelectedVertex;
        newFeature.q.x = posX;
        newFeature.q.y = posY;
				
        sourceImageFeatures.push_back(newFeature);

        isVertexSelectedInSourceImage = false;
        previousSourceImageActions.push(SOURCE_FEATURE_ADDED);
        cout << "Number of source image features: "
            << sourceImageFeatures.size()
            << endl;
      } else {
        sourceImageSelectedVertex.x = posX;
        sourceImageSelectedVertex.y = posY;
        isVertexSelectedInSourceImage = true;
        previousSourceImageActions.push(SOURCE_VERTEX_STACK_ADDED);
      }
	  previousActionTurns.push(SOURCE_IMAGE);
    } else if (posX >= 0.f) {// destination image
      if (isVertexSelectedInDestinationImage) {
        Feature newFeature;
        newFeature.p = destinationImageSelectedVertex;
        newFeature.q.x = posX;
        newFeature.q.y = posY;
			
        destinationImageFeatures.push_back(newFeature);

        isVertexSelectedInDestinationImage = false;
        previousDestinationImageActions.push(DESTINATION_FEATURE_ADDED);
        cout << "Number of destination image features: "
            << destinationImageFeatures.size() << endl;
      } else {
        destinationImageSelectedVertex.x = posX;
        destinationImageSelectedVertex.y = posY;
				
        isVertexSelectedInDestinationImage = true;
        previousDestinationImageActions.push(DESTINATION_VERTEX_STACK_ADDED);
      }
      previousActionTurns.push(DESTINATION_IMAGE);
    }
  } else { 
    mouseButton = -1;
  }
}

void uploadFeatureFile(string featureFile, string type) {
  if (featureFile != "" && (type != "source" || type != "destination")) {
    string upperCaseImageType = ((type == "source")
        ? "Source" : "Destination");

    ifstream in;
    in.open(featureFile);
    if (!in.good()) {
      cout << upperCaseImageType << " image features could not be opened. "
          << "The corresponding image will be opened without features.\n";
    } 
    while (in.good()) {
      string px, py, qx, qy;
      in >> px >> py >> qx >> qy;

      if (px != "" && py != "" && qx != "" && qy != "") {
        Feature currentFeature;
        currentFeature.p.x = atof(px.c_str()) - (type == "source" ? 1.f : 0.f);
        currentFeature.p.y = atof(py.c_str());
        currentFeature.q.x = atof(qx.c_str()) - (type == "source" ? 1.f : 0.f);
        currentFeature.q.y = atof(qy.c_str());
        if (type == "source") {
          sourceImageFeatures.push_back(currentFeature);
          previousSourceImageActions.push(SOURCE_VERTEX_STACK_ADDED);
          previousSourceImageActions.push(SOURCE_FEATURE_ADDED);
        } else if (type == "destination") {
          destinationImageFeatures.push_back(currentFeature);
          previousDestinationImageActions.push(DESTINATION_VERTEX_STACK_ADDED);
          previousDestinationImageActions.push(DESTINATION_FEATURE_ADDED);
        }
      }			
    }
    in.close();
  }
}

void outputFeatureFiles(bool withOverride) {
  if (sourceImageFeatures.size() != destinationImageFeatures.size()
      && !withOverride) {
    cout << "Could not generate output feature files. The number of features "
        << "specified for both images do not match. Press 'f' to "
        << "force-produce.\n";
  } else {
    if (withOverride) {
      cout << "Override. The number of features specified for both images do "
          << "not match, but feature files will still be created.\n";
    }
    string suffix = "_v" + intToString(currentOutputFileCount) + ".feat";
    string finalSourceImageOutputFileName = outputFilesSubDirectory 
        + sourceImageFeaturesOutputFileName + suffix;
    string finalDestinationImageOutputFileName = outputFilesSubDirectory 
        + destinationImageFeaturesOutputFileName + suffix;

    ofstream out;
    out.open(finalSourceImageOutputFileName);
    for (int i = 0; i < sourceImageFeatures.size(); ++i) {
      Feature currentFeature = sourceImageFeatures[i];
      out << currentFeature.p.x + 1.f << " ";
      out << currentFeature.p.y 		<< " ";
      out << currentFeature.q.x + 1.f << " ";
      out << currentFeature.q.y;
      if (i != sourceImageFeatures.size() - 1) {
        out << endl;
      }
    }
    out.close();
		
    out.open(finalDestinationImageOutputFileName);
    for (int i = 0; i < destinationImageFeatures.size(); ++i) {
      Feature currentFeature = destinationImageFeatures[i];
      out << currentFeature.p.x << " ";
      out << currentFeature.p.y << " ";
      out << currentFeature.q.x << " ";
      out << currentFeature.q.y;
      if (i != destinationImageFeatures.size() - 1) {
        out << endl;
      }
    }		
    out.close();

    cout << "Successfully generated output files.\n";
    currentOutputFileCount++;
  }
}

void staggerUploadedPreviousActions() {
  int sourceSize = sourceImageFeatures.size();
  int destinationSize = destinationImageFeatures.size();

  if (sourceSize != 0 || destinationSize != 0) {
    int smallerNumberOfFeatures = min(sourceSize,destinationSize);

    if (sourceSize > smallerNumberOfFeatures) {
      cout << "Uploaded source image features file has more features ("
          << sourceSize << " versus " << destinationSize <<  ").\n";
      for (int i = 0; i < sourceSize - smallerNumberOfFeatures; ++i) {
        previousActionTurns.push(SOURCE_IMAGE);
        previousActionTurns.push(SOURCE_IMAGE);
      }
    } else if (destinationSize > smallerNumberOfFeatures) {
      cout << "Uploaded destination image features file has more features ("
          << destinationSize << " versus " << sourceSize << ").\n";
      for (int i = 0; i < destinationSize - smallerNumberOfFeatures; ++i) {
        previousActionTurns.push(DESTINATION_IMAGE);
        previousActionTurns.push(DESTINATION_IMAGE);
      }
    } else {
      cout << "Uploaded files have the same number of features.\n";
    }
		
    for (int i = 0; i < smallerNumberOfFeatures; ++i) {
      previousActionTurns.push(SOURCE_IMAGE);
      previousActionTurns.push(SOURCE_IMAGE);
      previousActionTurns.push(DESTINATION_IMAGE);
      previousActionTurns.push(DESTINATION_IMAGE);
    }
  }
}

void undoLastAction() {
  if (!previousActionTurns.empty()) {
    PreviousActionTurn previousActionTurn = previousActionTurns.top();
    previousActionTurns.pop();
			
    PreviousAction previousAction;

    if (previousActionTurn == SOURCE_IMAGE) {
      previousAction = previousSourceImageActions.top();
      previousSourceImageActions.pop();

      if (previousAction == SOURCE_VERTEX_STACK_ADDED) {
        isVertexSelectedInSourceImage = false;
      } else if (previousAction == SOURCE_FEATURE_ADDED) {
        Feature lastFeature =
            sourceImageFeatures[sourceImageFeatures.size() - 1];
        sourceImageFeatures.pop_back();
        sourceImageSelectedVertex = lastFeature.p;
        isVertexSelectedInSourceImage = true;
      }
    } else if (previousActionTurn == DESTINATION_IMAGE) {
      previousAction = previousDestinationImageActions.top();
      previousDestinationImageActions.pop();
		
      if (previousAction == DESTINATION_VERTEX_STACK_ADDED) {
        isVertexSelectedInDestinationImage = false;
      } else if (previousAction == DESTINATION_FEATURE_ADDED) {
        Feature lastFeature =
            destinationImageFeatures[destinationImageFeatures.size() - 1];
            destinationImageFeatures.pop_back();
            destinationImageSelectedVertex = lastFeature.p;
            isVertexSelectedInDestinationImage = true;
      }
    }
  }
}

void screenshot() {
  string outputFile = screenshotSubDirectory + screenshotFileName;
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  float out[3 * w * h];

  RGBColor BG(0,0,0);
  SimpleImage shot(w, h, BG);
  glReadPixels(0, 0, w, h, GL_RGB, GL_FLOAT, &out[0]);
  for (int y = 0; y < h; ++y) {
    for(int x = 0; x < w; ++x) {
      int index = (3 * w * y) + 3*x;
      float red = out[index];
      float green = out[index + 1];
      float blue = out[index + 2];
      shot.setPixel(x,y,RGBColor(red, green, blue));
    }
  }
  outputFile += "_v";
  outputFile += intToString(currentScreenshotCount);
  outputFile += ".png";
  if (shot.WritePNG(outputFile)) {
    currentScreenshotCount++;
    cout << "Successfully generated screenshot.\n";
  }
}

void cleanUp() {
  delete[] imageTextures;
}

void parseArguments(int argc, char **argv) {
  if (argc < 3 || argc > 13) {
    printf("usage: ./featureAdder <source PNG image> <destination "
      "PNG image>\n");
    printf("\t[--es <source image input file name>] [--ed "
        "<destination image input file name>]\n");
	printf("\t[--os <source image output file name>] "
        "[--od <destination image output file name>]\n");
	printf("\t[--s <screenshot image output file name>]\n");
	exit(0);
  } else {
    if (argc % 2 == 0) {
      cout << "Missing flag or argument.\n";
      exit(0);
    } else {
      sourceImageFileName = argv[1];
      destinationImageFileName = argv[2];
      for (int i = 3; i < argc; i += 2) {
        string flag = argv[i];
        string flagArgument = argv[i+1];

        if (flag[0] != '-') {
          cout << "Missing flag for argument.\n";
          exit(0);
        } else if (flagArgument[0] == '-') {
          cout << "Missing argument to flag.\n";
          exit(0);
        } else {
          flag = flag.substr(2, flag.size()-2);
          if (flag == "es") {
            sourceImageFeaturesFileName = flagArgument;
          } else if (flag == "ed") {
              destinationImageFeaturesFileName = flagArgument;
          } else if (flag == "os") {
              sourceImageFeaturesOutputFileName = flagArgument;
          } else if (flag == "od") {
              destinationImageFeaturesOutputFileName = flagArgument;
          } else if (flag == "s") {
              screenshotFileName = flagArgument;
          } else {
              cout << "Unknown flag.\n";
              exit(0);
          }
        }
      }
    }
  }
}
