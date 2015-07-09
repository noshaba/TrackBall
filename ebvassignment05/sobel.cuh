//Macro that makes it easier to spot errors
//Sample usage: CHECK_CUDA(cudaMalloc<int>(&gpu_source, size));
#define CHECK_CUDA(call) do { \
    cudaError_t status = call; \
    if( status != cudaSuccess ) { \
        fprintf(stderr, "CUDA Error at line %d in %s: %s\n", __LINE__, __FILE__, cudaGetErrorString(status)); \
        exit(1); \
    } \
} while(0)

//Computes the sobel-magnitude with \srcImg as input.
//dstImg(y,x) is the rounded vector length of the sobel
//vector computed from \scrImg(y,x) and its neighbors.
//
//This function will prepare the launch of the kernel.
//Allocate memory, copy source image to device memory,
//launch the kernel and grab the result (save to dstImg)
void sobel (Mat_<uchar>& dstImg, const Mat_<uchar>& srcImg);

//Optional usage
static inline int round_up(int x, int y)
{
    return (x + y-1) / y;
}

//Optional usage
static __device__ __forceinline__ int in_img(int x, int y, int rows, int cols)
{
    return x >= 0 && x < cols && y >= 0 && y < rows;
}

//Optional usage
static __constant__ int sobel_filter_x[3][3] = {
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1,
};

//Optional usage
static __constant__ int sobel_filter_y[3][3] = {
    1,  2,  1,
    0,  0,  0,
    -1, -2, -1,
};

