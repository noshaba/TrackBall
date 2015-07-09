#include "central.h"
#include <cuda_runtime_api.h>
#include "sobel.cuh"

__device__ void sobel1Px (Mat_<uchar>& dstImg, const Mat_<uchar>& srcImg, int x2, int y2)
{
	bool inside = x2 -1 >= 0 && x2 +1 < srcImg.cols && y2 -1 >= 0 && y2 +1 < srcImg.rows;

	//sobel x filter
	int sX = inside ?
		(
		(-srcImg(y2 - 1, x2 -1) - 2 * srcImg(y2,x2-1) - srcImg(y2+1,x2-1)
		+ srcImg(y2 - 1, x2 +1) + 2 * srcImg(y2,x2+1) + srcImg(y2+1,x2+1)) * 0.125f) : 0;

	//sobel y filter
	int sY = inside ?
		(
		(-srcImg(y2 - 1, x2 -1) - 2 * srcImg(y2-1,x2) - srcImg(y2-1,x2+1)
		+ srcImg(y2 + 1, x2 -1) + 2 * srcImg(y2+1,x2) + srcImg(y2+1,x2+1)) * 0.125f) : 0;

	// filtered pixel
	dstImg(y2,x2) = sobelCode(sX,sY);
}

__global__ void sobelKernel (uchar* dstImg, uchar* srcImg, int rows, int cols, int totalRows, int totalCols)
{
    //TODO: implement the sobel operator (6P)

	
	bool inside = 	(cols -1 >= 0) 		&& 
			(cols+1 < totalCols) 	&& 
			(rows -1 >= 0 && rows +1 < totalRows);
	

	//sobel x filter
	int sX = inside ?
		(
		(-srcImg[rows - 1, cols -1] - 2 * srcImg(rows,cols-1) - srcImg(rows+1,cols-1)
		+ srcImg(rows - 1, cols +1) + 2 * srcImg(rows,cols+1) + srcImg(rows+1,cols+1)) * 0.125f) : 0;

	//sobel y filter
	int sY = inside ?
		(
		(-srcImg(rows - 1, cols -1) - 2 * srcImg(rows-1,cols) - srcImg(rows-1,cols+1)
		+ srcImg(rows + 1, cols -1) + 2 * srcImg(rows+1,cols) + srcImg(rows+1,cols+1)) * 0.125f) : 0;

	// filtered pixel
	dstImg(rows,cols) = sobelCode(sX,sY);
}


void sobel (Mat_<uchar>& dstImg, const Mat_<uchar>& srcImg)
{
    //TODO: implement (4P)

	int imgSize = srcImg.rows*srcImg.cols;
	Mat_<uchar> *srcImg_d;
	Mat_<uchar> *dstImg_d;
	cudaMalloc((void **) &srcImg_d,imgSize); 
	cudaMalloc((void **) &dstImg_d,imgSize);

	cudaMemcpy(srcImg_d, srcImg, imgSize, cudaMemcpyHostToDevice);
	cudaMemcpy(dstImg_d, dstImg, imgSize, cudaMemcpyHostToDevice);

	assert (dstImg.size() == srcImg.size());

	//edge detection
	for (int y2 = 0; y2 < dstImg.rows;y2++)
	{
		for (int x2 = 0; x2 <dstImg.cols; x2++)
			sobelKernel <<< srcImg.rows, srcImg.cols >>> (dstImg_d,srcImg_d,y2,x2,srcImg.rows,srcImg.cols);
	}
}
