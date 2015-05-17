#ifndef SOBEL_H_INCLUDED
#define SOBEL_H_INCLUDED

#include <opencv2/opencv.hpp>
using namespace cv;

//! A coded (0,0) vector
extern int sobel0;

inline double sq(double x) {return x*x;}

//! code a vector of sobel responses into a single number
inline int sobelCode (int sobelX, int sobelY)
{
    return (sobelX+128) + ((sobelY+128)<<8);
}

//! extract the sobel vector's x component from the coded vector
inline int sobelXUncode (int sobelCoded)
{
    return (sobelCoded & 255)-128;
}

//! extract the sobel vector's y component from the coded vector
inline int sobelYUncode (int sobelCoded)
{
    return (sobelCoded>>8)-128;
}

//! Returns the sobel vector at (x,y) of a coded sobel image (pixel type int)
inline void sobelVector (const Mat_<ushort>& sobelImg, int x, int y, int& sobelX, int& sobelY)
{
    int sobelXYCoded = sobelImg (y, x);
    
    sobelX = sobelXUncode (sobelXYCoded);
    sobelY = sobelYUncode (sobelXYCoded);
}



//! Slow implementation of the sobelX/Y filter
/*! Computes the sobelX and sobelY filter on \c srcImg (scaled by 1/8)
 codes the result for each pixel into a single \c ushort number
 with \c sobelCode and stores the result into \c dstImg.
 \c dstImg is a 32bit int image of same format as \c srcImg.
 The 1-pixel border of \c dstImg is set to \c sobel0 indicating
 a (0,0) sobel vector because the filter result is undefined there.
 */
void sobel (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg);

//! Core routine for \c sobel
/*! Compute coded sobel vector \c dstImg(y2,x2) from \c srcImg
 */
void sobel1Px (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg, int x2, int y2);


//! Fast C implementation of \c sobelSlow
void sobelFast (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg);

//! Core routine for \c sobelFast
/*! Compute coded sobel vector \c *p from *pSrc and neighbours.
    \c sys is the memory offset between two source lines
    in uchar's. Does not need to handle image borders.
 */
void sobel1PxFast (ushort* p, const uchar* pSrc, int sys);



//! openMP wrapper that calls \c sobelFast in a parallel section
void sobelFastOpenMP (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg);


#endif
