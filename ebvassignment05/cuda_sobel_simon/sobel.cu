#include "central.h"
#include <cuda_runtime_api.h>
#include "sobel.cuh"


__global__ void sobelKernel (uchar* dstImg, uchar* srcImg, int rows, int cols)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    int offset = x+y * (gridDim.x * blockDim.x);

    bool inside = in_img(x, y, rows, cols);

    int sX = inside ?
            ((-srcImg[offset-cols-1] - 2 * srcImg[offset-1] - srcImg[offset+cols-1]
            + srcImg[offset-cols+1] + 2 * srcImg[offset+1] + srcImg[offset+cols+1]) * 0.125f) : 0;

    int sY = inside ?
            ((-srcImg[offset-cols-1] - 2 * srcImg[offset-cols] - srcImg[offset-cols+1]
            + srcImg[offset+cols-1] + 2 * srcImg[offset+cols] + srcImg[offset+cols+1]) * 0.125f) : 0;

    sX = (int) floor (sX+3.5/8);
    sY = (int) floor (sY+3.5/8);

    dstImg[offset] = (sX+128) + ((sY+128)<<8);
}


void sobel (Mat_<uchar>& dstImg, const Mat_<uchar>& srcImg)
{
    int size = dstImg.rows * dstImg.cols;

    uchar *d_dstImgData;
    uchar *d_srcImgData;

    cudaMalloc((void**) &d_dstImgData, size);
    cudaMalloc((void**) &d_srcImgData, size);

    cudaMemcpy(d_srcImgData, srcImg.data, size, cudaMemcpyHostToDevice);

    dim3 threads(16,16);
    dim3 blocks(dstImg.rows/threads.x, dstImg.cols/threads.y);

    sobelKernel<<<blocks,threads>>>(d_dstImgData, d_srcImgData, dstImg.rows, dstImg.cols);

    cudaDeviceSynchronize();
    cudaMemcpy(dstImg.data, d_dstImgData, size, cudaMemcpyDeviceToHost);

    cudaFree(d_srcImgData);
    cudaFree(d_dstImgData);
}
