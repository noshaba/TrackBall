#ifndef SOBELGUI_H
#define SOBELGUI_H

#include "central.h"
#include "gui.h"
#include "sobel.h"


//! Paints a coded sobel vector image 
/*! It paints in a way where the vector
    length corresponds to value and vector angle to hue
    the visualization is modulo PI
    Pixels with a sobellength below threshold are shown as black.
*/
void paintSobelImg (Mat& sobelImgDisplay, const Mat_<ushort>& sobelImg, int threshold=0);

//! Compares two int images and gives an error message if pixel differ
bool compareImages (const Mat_<ushort>& trueImg, const Mat_<ushort>& testImg, const char* trueName, const char* testName);


//! Main program for Edge detection.
void detectEdges (const std::vector<std::string>& images, bool doSave);

#endif
