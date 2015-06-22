#ifndef HCGUI_H
#define HCGUI_H

#include "gui.h"
#include "houghCircle.h"



//! GUI: Paint several circles onto 'overlay'
void paintCircles (Mat& overlay, const vector<HoughCircle::Circle>& circles);


//! Print center / radius for all circles
void printCircles (const vector<HoughCircle::Circle>& circles);

//! Main program for Hough Circle Transform.
void trackBall (const vector<string>& images, bool doSave);

#endif
