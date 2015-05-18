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
    // TODO: Implement (3P with sobel)
    // Check with testCoreRoutines that everything is correct.
    // No need to optimize here (this will be done in sobelFast)
    // Use sobelRound for rounding to give exact the same result
    // we have in the optimized routines.
	int sX, sY;
	bool inside = x2 - 1 >= 0 && x2 + 1 < srcImg.cols && y2 - 1 >= 0 && y2 + 1 < srcImg.rows;

	sX = inside ?
		sobelRound(
		(-srcImg(y2 - 1, x2 - 1) - 2 * srcImg(y2, x2 - 1) - srcImg(y2 + 1, x2 - 1)
		+ srcImg(y2 - 1, x2 + 1) + 2 * srcImg(y2, x2 + 1) + srcImg(y2 + 1, x2 + 1)) * 0.125f) : sobel0;

	sY = inside ?
		sobelRound(
		(srcImg(y2 - 1, x2 - 1) + 2 * srcImg(y2 - 1, x2) + srcImg(y2 - 1, x2 + 1)
		- srcImg(y2 + 1, x2 - 1) - 2 * srcImg(y2 + 1, x2) - srcImg(y2 + 1, x2 + 1)) * 0.125f) : sobel0;

	dstImg(y2, x2) = sobelCode(sX,sY);
}


void sobel (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
    // TODO: implement (with sobel1p)
    
    assert (dstImg.size()==srcImg.size());

	for (int y2 = 0; y2 < dstImg.rows; y2++)
		for (int x2 = 0; x2 < dstImg.cols; x2++)
			sobel1Px(dstImg, srcImg, x2, y2);
}


void sobel1PxFast (ushort* p, const uchar* pSrc, int sys)
{
    // TODO: implement (4P)
    // Check using testCoreRoutines that both routines compute
    // the same result
    // Use ((val+3)>>3) for divison by 8 and rounding to give exact the same result
    // we have in the unoptimized routines using sobelRound(...)
}


void sobelFast (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
    // TODO: implement (2P)
    // Provide a C++ implementation that is as fast as reasonable
    // Keep in mind that dstImg pixels are of type ushort
    assert (dstImg.size()==srcImg.size());
}




void sobelFastOpenMP (Mat_<ushort>& dstImg, const Mat_<uchar>& srcImg)
{
#ifdef NOOPENMP
    std::cerr << "Warning: OpenMP not activated." << endl;
#endif
    //TODO: implement (1P including changes in sobelFast)
    // Put the parallelization pragmas except for the parallel section into sobelSIMD
    // Put the parallel section here so sobelFastOpenMP will run in parallel, 
    // whereas sobelFast itself will not as it is not inside a parallel section.
}
