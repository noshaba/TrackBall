#include "central.h"
#include <cuda_runtime_api.h>
#include "sobel.cuh"

__global__ void sobelKernel (uchar* dstImg, uchar* srcImg, int rows, int cols)
{
    //TODO: implement the sobel operator (6P)
	unsigned int x = blockDim.x * blockIdx.x + threadIdx.x;
	unsigned int y = blockDim.y * blockIdx.y + threadIdx.y;
	unsigned int step = gridDim.x * blockDim.x; 
	
	double sX = 0, sY = 0;
	
	if(in_img(x-1,y-1,rows,cols) && in_img(x+1,y+1,rows,cols)) {
		// (x,y) = step * y + x
		sX = (-srcImg[step * (y - 1) + x - 1] - 2 * srcImg[step * y + x - 1] - srcImg[step * (y + 1) + x - 1]
			 + srcImg[step * (y - 1) + x + 1] + 2 * srcImg[step * y + x + 1] + srcImg[step * (y + 1) + x + 1]) * 0.125;
			 
		sY = (-srcImg[step * (y - 1) + x - 1] - 2 * srcImg[step * (y - 1) + x] - srcImg[step * (y - 1) + x + 1]
			 + srcImg[step * (y + 1) + x - 1] + 2 * srcImg[step * (y + 1) + x] + srcImg[step * (y + 1) + x + 1]) * 0.125;
	}
	
	// save sobel length in image
	if(in_img(x,y,rows,cols))
		dstImg[step * y + x] = sqrt(sX * sX + sY * sY);
}

void sobel (Mat_<uchar>& dstImg, const Mat_<uchar>& srcImg)
{
    //TODO: implement (4P)
	
	unsigned int data_length = dstImg.rows * dstImg.cols;
	size_t size = data_length * sizeof(uchar);
	
	uchar *gpuSrcImg, *gpuDstImg;
	
	// allocate memory for the images
	CHECK_CUDA(cudaMalloc((void**) &gpuSrcImg, size));
	CHECK_CUDA(cudaMalloc((void**) &gpuDstImg, size));
	
	// transfer the initialized source image to the device
	CHECK_CUDA(cudaMemcpy(gpuSrcImg, srcImg.data, size, cudaMemcpyHostToDevice));
	
	dim3 threads(16, 16);
	dim3 blocks(round_up(dstImg.cols, threads.x), round_up(dstImg.rows, threads.y));
	
	sobelKernel<<<blocks, threads>>>(gpuDstImg, gpuSrcImg, srcImg.rows, srcImg.cols);
	
	// copy results back to the host (implies cudaDeviceSynchronize())
	CHECK_CUDA(cudaMemcpy(dstImg.data, gpuDstImg, size, cudaMemcpyDeviceToHost));
	
	// free the memory
	CHECK_CUDA(cudaFree(gpuSrcImg));
	CHECK_CUDA(cudaFree(gpuDstImg));
	
	CHECK_CUDA(cudaDeviceReset());
}
