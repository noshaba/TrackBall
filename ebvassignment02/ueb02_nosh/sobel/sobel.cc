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

	int sX = inside ?
		sobelRound(
		(-srcImg(y2 - 1, x2 - 1) - 2 * srcImg(y2, x2 - 1) - srcImg(y2 + 1, x2 - 1)
		+ srcImg(y2 - 1, x2 + 1) + 2 * srcImg(y2, x2 + 1) + srcImg(y2 + 1, x2 + 1)) * 0.125f) : 0;

	int sY = inside ?
		sobelRound(
		(-srcImg(y2 - 1, x2 - 1) - 2 * srcImg(y2 - 1, x2) - srcImg(y2 - 1, x2 + 1)
		+ srcImg(y2 + 1, x2 - 1) + 2 * srcImg(y2 + 1, x2) + srcImg(y2 + 1, x2 + 1)) * 0.125f) : 0;

	dstImg(y2, x2) = sobelCode(sX,sY);
}


void sobel (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
    assert (dstImg.size()==srcImg.size());

	for (int y2 = 0; y2 < dstImg.rows; y2++)
		for (int x2 = 0; x2 < dstImg.cols; x2++)
			sobel1Px(dstImg, srcImg, x2, y2);
}

/*********************************************** 4 da lulz ***********************************************/

// almost twice as fast as 'sobelFast' without preprocessor directives :D

#define SOBEL1PXFAST(p, pSrc, sys)\
{\
	int sX = (-(int)pSrc[-sys - 1] - ((int)pSrc[-1] << 1) - (int)pSrc[+sys - 1]\
			 + (int)pSrc[-sys + 1] + ((int)pSrc[1] << 1) + (int)pSrc[+sys + 1] + 3) >> 3;\
	\
	int sY = (-(int)pSrc[-sys - 1] - ((int)pSrc[-sys] << 1) - (int)pSrc[-sys + 1]\
			 + (int)pSrc[+sys - 1] + ((int)pSrc[+sys] << 1) + (int)pSrc[+sys + 1] + 3) >> 3;\
	\
	*p = sobelCode(sX, sY);\
}

#define SOBELFAST(dstImg, srcImg)\
{\
	assert (dstImg.size()==srcImg.size());\
	\
	ushort *p, *pEnd, *pLine;\
	const uchar *pSrc;\
	int sys = srcImg.step[0] / srcImg.step[1];\
	\
	for (p = dstImg.ptr<ushort>(0), pEnd = p + dstImg.cols; p < pEnd; p++) \
		*p = sobel0;\
	\
	for (int y2 = 1; y2 < dstImg.rows - 1; y2++) {\
		pLine = dstImg.ptr<ushort>(y2);\
		pLine[0] = sobel0;\
		\
		for (pSrc = srcImg.ptr(y2) + 1, p = pLine + 1, pEnd = p + dstImg.cols - 2; p < pEnd; p++, pSrc++) \
			SOBEL1PXFAST(p, pSrc, sys);\
		\
		pLine[dstImg.cols - 1] = sobel0;\
	}\
	\
	for (p = dstImg.ptr<ushort>(dstImg.rows - 1), pEnd = p + dstImg.cols; p<pEnd; p++) \
		*p = sobel0;\
}

/*********************************************************************************************************/

void sobel1PxFast(ushort* p, const uchar* pSrc, int sys)
{
	// SOBEL1PXFAST(p, pSrc, sys);
	int sX = (-(int)pSrc[-sys - 1] - ((int)pSrc[-1] << 1) - (int)pSrc[+sys - 1]
		     + (int)pSrc[-sys + 1] + ((int)pSrc[1] << 1) + (int)pSrc[+sys + 1] + 3) >> 3;
		
	int sY = (-(int)pSrc[-sys - 1] - ((int)pSrc[-sys] << 1) - (int)pSrc[-sys + 1]
			 + (int)pSrc[+sys - 1] + ((int)pSrc[+sys] << 1) + (int)pSrc[+sys + 1] + 3) >> 3; 
		
		*p = sobelCode(sX, sY); 
}

void sobelFast (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
	// SOBELFAST(dstImg, srcImg);
	assert(dstImg.size() == srcImg.size()); 
	
	ushort *p, *pEnd, *pLine; 
	const uchar *pSrc; 
	int sys = srcImg.step[0] / srcImg.step[1];

	for (p = dstImg.ptr<ushort>(0), pEnd = p + dstImg.cols; p < pEnd; p++)
		*p = sobel0;

	for (int y2 = 1; y2 < dstImg.rows - 1; y2++) {

		pLine = dstImg.ptr<ushort>(y2);
		pLine[0] = sobel0;

		for (pSrc = srcImg.ptr(y2) + 1, p = pLine + 1, pEnd = p + dstImg.cols - 2; p < pEnd; p++, pSrc++)
			sobel1PxFast(p, pSrc, sys);

		pLine[dstImg.cols - 1] = sobel0;
	}

	for (p = dstImg.ptr<ushort>(dstImg.rows - 1), pEnd = p + dstImg.cols; p < pEnd; p++)
		*p = sobel0;
}

void sobelFastOpenMP (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
#ifdef NOOPENMP
	std::cerr << "Warning: OpenMP not activated." << endl;
#endif
	//TODO: implement (1P including changes in sobelFast)
	// Put the parallel section here so sobelFastOpenMP will run in parallel, 
	// whereas sobelFast itself will not as it is not inside a parallel section.

	assert(dstImg.size() == srcImg.size());

	// What am I doing wrong.... Q_Q

	#pragma omp parallel
	{
		ushort *p, *pEnd, *pLine;
		const uchar *pSrc;
		int sys = srcImg.step[0] / srcImg.step[1];

		#pragma omp for
		for (p = dstImg.ptr<ushort>(0), pEnd = p + dstImg.cols; p < pEnd; p++)
			*p = sobel0;

		#pragma omp for
		for (int y2 = 1; y2 < dstImg.rows - 1; y2++) {

			pLine = dstImg.ptr<ushort>(y2);
			pLine[0] = sobel0;

			for (pSrc = srcImg.ptr(y2) + 1, p = pLine + 1, pEnd = p + dstImg.cols - 2; p < pEnd; p++, pSrc++)
				sobel1PxFast(p, pSrc, sys);

			pLine[dstImg.cols - 1] = sobel0;
		}

		#pragma omp for
		for (p = dstImg.ptr<ushort>(dstImg.rows - 1), pEnd = p + dstImg.cols; p < pEnd; p++)
			*p = sobel0;
	}
}
