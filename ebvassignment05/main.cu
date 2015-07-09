#include <iostream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
using namespace cv;

#ifdef WIN32
#include <time.h>
#endif

#include "sobel.cu"// important to include .cu file, not header file

void detectEdges (const std::vector<std::string>& images);
std::string resultName (const std::string& fname, const std::string& resultTxt);

int main (int argc, char** argv)
{
    std::vector<std::string> fname;
    enum {FULL, EDGES, CIRCLES, LINES, CALIBRATE, TRACK} mode = FULL;

    for (int i=1; i<argc; i++)
        if (std::string(argv[i])==std::string("-edges")) mode = EDGES;
        else if (argv[i][0]!='-') fname.push_back (std::string(argv[i]));
        else {
            std::cerr << "Unknown option " << argv[i] << std::endl;
            return 2;
        }
    if (fname.empty()) {
        std::cout << "No image specified." << std::endl;
        return 2;
    }
    if (mode==EDGES) {
        cudaFree(0);//initialize CUDA
        detectEdges (fname);
    }
    else {
        std::cout << "Illegal command " << std::endl;
        return 1;
    }
    return 0;
}

void detectEdges (const std::vector<std::string>& images)
{
    if (images.size()==0) return;
    for(int imageCtr=0; imageCtr<(int) images.size(); imageCtr++) {
        std::string fname = images[imageCtr];
        Mat_<uchar> srcImg = imread (fname, 0); // load image as greyscale
        if (srcImg.empty()) {
            std::cerr << "Could not find " << fname << std::endl;
            continue;
        }
        Mat_<uchar> sobelImg1 = Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC1);

        int64 time, time2;
        time = cvGetTickCount();
        sobel (sobelImg1, srcImg);
        time2 = cvGetTickCount();

        double tms = 0.001*(time2-time)/cvGetTickFrequency();

        std::cout << "Overall computation time: " << tms << "ms" << std::endl;
        std::string rname = resultName (fname, ".sobel");
        imwrite (rname, sobelImg1);   // save the resulting image
        std::cout << "Saving " << rname << std::endl;
    }
}

std::string resultName (const std::string& fname, const std::string& resultTxt)
{
    int n = fname.length();
    int idx = fname.rfind(".");
    if (idx<0) idx=n;
    return fname.substr(0,idx) + resultTxt + fname.substr(idx, n-idx);
}
