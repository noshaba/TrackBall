#ifndef GUI_H
#define GUI_H

#include "central.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/types_c.h>
using namespace cv;

#ifdef _LINUX
#include <X11/keysym.h>
#else
// Under Windows/MAC OS we don't have the X key konstants
// use the following keys instead.
#define XK_Left 'h'
#define XK_Right 'k'
#define XK_Up 'u'
#define XK_Down 'j'
#define XK_Tab (9)
#define XK_Return (13)
#define XK_Escape (27)
#endif


//! GUI: paint a red cross at 'xC,yC' into overlay
void paintCross (Mat& overlay, int xC, int yC, CvScalar color);

//! Scale the intensity values of the hough image for displaying.
/*! After scaling \c topLevel corresponds to white and 0 to black. */
void scaleHoughImage (Mat_<ushort>& houghImg, int topLevel);

//! Appends a \c resultTxt to \c fname before the file extension
std::string resultName (const std::string& fname, const std::string& resultTxt);

#endif
