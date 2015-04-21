#ifdef WIN32
#define _USE_MATH_DEFINES
#include <stdafx.h>
#endif

#include "cv.h"
#include "highgui.h"
#include <stdio.h>

#include <assert.h>
#include <math.h>
#include <iostream>
#include <vector>
using namespace std;


// A class representing an detected pea
class Pea 
{
public:
  Pea (int x, int y) :x(x), y(y) {};
  Pea () :x(0), y(0) {};  

  // The peas position (any point of the pea)
  int x, y;  
};

  
  
      


//******************************************************************
// GUI code
//******************************************************************

// GUI: paint a red cross at 'xC,yC' into overlay
void paintCross (IplImage* overlay, int xC, int yC)
{
  int cL = 5;
  CvPoint p0 = cvPoint (xC-cL, yC-cL);
  CvPoint p1 = cvPoint (xC+cL, yC+cL);
  cvLine (overlay, p0, p1, CV_RGB (255, 0, 0));
  p0 = cvPoint (xC+cL, yC-cL);
  p1 = cvPoint (xC-cL, yC+cL);
  cvLine (overlay, p0, p1, CV_RGB (255, 0, 0));
}


// Paint a list of cross at the positions given by 'peas'
void paintPeas (IplImage* overlay, const vector<Pea>& pea)
{
  for (int i=0; i<(int) pea.size(); i++) 
    paintCross (overlay, pea[i].x, pea[i].y);  
}

unsigned char& pixel(IplImage* img, int x, int y)
{
  return *((unsigned char*) cvPtr2D (img, y, x));
}


// Find all peas in 'img' and return a list of their positions.
// The 'Pea.x,Pea.y' entries in 'pea' correspond to an arbitrary
// point of the pea.
//
// HELP:
//  Image width: img->width
//  Image height: img->height
//  Image pixel (x,y): pixel (img, x, y)
//  0 corresponds to black, 255 to white
// An object can be added to a list with 'pea.push_back(...)'

void findPeas (IplImage* img, vector<Pea>& pea)
{
}




int main (int argc, char** argv)
{    
  cvNamedWindow ("Counting the number of peas", CV_WINDOW_AUTOSIZE);

  if (argc<=1) {
    cerr << "peas <image1> ..." << endl;    
    return 1;
  }  

  int imageCtr=0;
  while (imageCtr<argc-1) {   
    char* fname = argv[imageCtr+1];    
    cout << " Loading " << fname << endl;    
    IplImage* srcImg = cvLoadImage (fname, 0); // load image as greyscale
    if (srcImg==NULL) {
      if (imageCtr==0) {
	cerr << "Could not find " << fname << endl;
	return 1;
      }      
      break;
    }
    IplImage* overlayImg = cvLoadImage (fname, 1); // load image as RGB for overlaying crosses

    vector<Pea> pea;

    // Do the computation
    findPeas (srcImg, pea);

    paintPeas (overlayImg, pea);    

    cvShowImage ("Counting the number of peas", overlayImg);
    cout << "We have " << pea.size() << " peas." << endl;    
    int key = cvWaitKey (0);   
    if (key=='q') break;    
    bool doSave = false;    
    if (doSave) {
      sprintf (fname, "result.%03d.jpg", imageCtr);
      cvSaveImage (fname, overlayImg);   // save the resulting image
    }
    cvReleaseImage (&srcImg);
    cvReleaseImage (&overlayImg);    

    imageCtr++;    
  }

  return 0;  
}


