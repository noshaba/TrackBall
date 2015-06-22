#include "gui.h"
#ifdef WIN32
#include <opencv2/core/matx.hpp>
#endif

void paintCross (Mat& overlay, int xC, int yC, CvScalar color)
{
    int cL = 5;
    Point p0 (xC-cL, yC-cL);
    Point p1 (xC+cL, yC+cL);
    line (overlay, p0, p1, color);
    Point p2 (xC+cL, yC-cL);
    Point p3 (xC-cL, yC+cL);
    line (overlay, p2, p3, color);
}



void scaleHoughImage (Mat_<ushort>& houghImg, int topLevel)
{
    for (int y=0; y<houghImg.rows; y++) {
        for (ushort* p    = houghImg[y], *pEnd = p + houghImg.cols; p<pEnd; p++)
            *p = (ushort) ((min((int) *p, topLevel)*(int) 65535)/topLevel);
    }
}


std::string resultName (const std::string& fname, const std::string& resultTxt)
{
  int n = fname.length();  
  int idx = fname.rfind(".");
  if (idx<0) idx=n;
  return fname.substr(0,idx) + resultTxt + fname.substr(idx, n-idx);
}


