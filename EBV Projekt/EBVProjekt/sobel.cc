#include "sobel.h"
#include "gui.h"
#ifndef NOOPENMP
#include <omp.h>
#endif

int sobel0 = sobelCode(0,0);

//! A special rounding variant compatible with the later fast implementation for Sobel
int sobelRound (float x) {
    return (int) floor (x+3.5/8);
}

void sobel1Px (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg, int x2, int y2)
{
	bool inside = x2 - 1 >= 0 && x2 + 1 < srcImg.cols && y2 - 1 >= 0 && y2 + 1 < srcImg.rows;

	// sobel x filter
	int sX = inside ?
		sobelRound(
		(-srcImg(y2 - 1, x2 - 1) - 2 * srcImg(y2, x2 - 1) - srcImg(y2 + 1, x2 - 1)
		+ srcImg(y2 - 1, x2 + 1) + 2 * srcImg(y2, x2 + 1) + srcImg(y2 + 1, x2 + 1)) * 0.125f) : 0;

	// sobel y filter
	int sY = inside ?
		sobelRound(
		(-srcImg(y2 - 1, x2 - 1) - 2 * srcImg(y2 - 1, x2) - srcImg(y2 - 1, x2 + 1)
		+ srcImg(y2 + 1, x2 - 1) + 2 * srcImg(y2 + 1, x2) + srcImg(y2 + 1, x2 + 1)) * 0.125f) : 0;

	// filtered pixel
	dstImg(y2, x2) = sobelCode(sX,sY);
}


void sobel (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
    assert (dstImg.size()==srcImg.size());

	// edge detection
	for (int y2 = 0; y2 < dstImg.rows; y2++)
		for (int x2 = 0; x2 < dstImg.cols; x2++)
			sobel1Px(dstImg, srcImg, x2, y2);
}

/*
* sobel1PxFast as macro function
* 
* Advantage:
* - makes the functions sobelFast and sobelFastOpenMP twice as fast due to forced inlining
* - less overhead since no type checking
* Disadvantage:
* - because of no type checking, weird things can happen - e.g. when used with expressions such as (1+2): #define square(a) a*a
*																												  square(1+2) --> 1+2*1+2 --> 1+2+2 --> 5 
* - less readable
*/

#define SOBEL1PXFAST(p, pSrc, sys)\
{\
	int sX = (-(int)pSrc[-sys - 1] - ((int)pSrc[-1] << 1) - (int)pSrc[+sys - 1]\
			 + (int)pSrc[-sys + 1] + ((int)pSrc[1]  << 1) + (int)pSrc[+sys + 1] + 3) >> 3;\
	\
	int sY = (-(int)pSrc[-sys - 1] - ((int)pSrc[-sys] << 1) - (int)pSrc[-sys + 1]\
			 + (int)pSrc[+sys - 1] + ((int)pSrc[+sys] << 1) + (int)pSrc[+sys + 1] + 3) >> 3;\
	\
	*p = sobelCode(sX, sY);\
}

void sobel1PxFast(ushort* p, const uchar* pSrc, int sys)
{
	// uses the sobel filter on one pixel
	 SOBEL1PXFAST(p, pSrc, sys);
}

void sobelFast (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
	assert(dstImg.size() == srcImg.size()); 
	
	ushort *p = nullptr, *pEnd = nullptr, *pLine = nullptr; 
	const uchar *pSrc = nullptr; 
	// step to neighbor
	int sys = srcImg.step[0] / srcImg.step[1];

	pLine = dstImg.ptr<ushort>(0);
	// apply sobel on first line of image - have to be 0 since undefined with a 3x3 filter
	#pragma omp for
	for (int y = 0; y < dstImg.cols; y++)
		pLine[y] = sobel0;

	// apply sobel on second to last - 1 line of image
	#pragma omp for
	for (int y2 = 1; y2 < dstImg.rows - 1; y2++) {
		// current line
		pLine = dstImg.ptr<ushort>(y2);
		// first pixel in line has to be 0 since undefined with a 3x3 filter
		pLine[0] = sobel0;

		// apply sobel filter on the current row, from pixel[1] to pixel[end-2]
		// no parallizing - too much overhead
		for (pSrc = srcImg.ptr(y2) + 1, p = pLine + 1, pEnd = p + dstImg.cols - 2; p < pEnd; p++, pSrc++)
			SOBEL1PXFAST(p, pSrc, sys);
		
		// last pixel in line has to be 0 since undefined with a 3x3 filter
		pLine[dstImg.cols - 1] = sobel0;
	}

	pLine = dstImg.ptr<ushort>(dstImg.rows - 1);
	// apply sobel on last line of image - have to be 0 since undefined with a 3x3 filter
	#pragma omp for
	for (int y = 0; y < dstImg.cols; y++)
		pLine[y] = sobel0;
}

void sobelFastOpenMP (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
#ifdef NOOPENMP
	std::cerr << "Warning: OpenMP not activated." << endl;
#endif
	assert(dstImg.size() == srcImg.size());

	// distribution on different cores
	#pragma omp parallel
	{
		sobelFast(dstImg, srcImg);
	}
}
