#include "sobelgui.h"
#include <iostream>
#ifndef NOOPENMP
#include <omp.h>
#endif


void paintSobelImg (Mat& sobelImgDisplay, const Mat_<ushort>& sobelImg, int threshold)
{
    double factor = 8;
    for (int y=0; y<sobelImg.rows; y++) for (int x=0; x<sobelImg.cols; x++) {
        int sobelX, sobelY;
        sobelVector (sobelImg, x, y, sobelX, sobelY);
        double sobelLen = factor*sqrt((double)(sobelX*sobelX+sobelY*sobelY));
        if (sobelLen<threshold) sobelLen=0;
        if (sobelLen>255) sobelLen=255;
        double sobelAngle = atan2 ((double) sobelY, (double) sobelX);
        double r = sobelLen*sq(sin(sobelAngle   ));
        double g = sobelLen*sq(sin(sobelAngle+  M_PI/3));
        double b = sobelLen*sq(sin(sobelAngle+2*M_PI/3));
        sobelImgDisplay.at<Vec3b> (y,x) = Vec3b((int) r, (int) g, (int) b);
    }
}

bool compareImages (const Mat_<ushort>& trueImg, const Mat_<ushort>& testImg, const char* trueName, const char* testName)
{
    if (trueImg.size()!=testImg.size()) {
        std::cerr << "ERROR: true image is " << trueImg.cols << "*" << trueImg.rows <<
        testName << " is" << testImg.cols << "*" << testImg.rows << std::endl;
        return false;
    }
    for (int y=0; y<trueImg.rows; y++) for (int x=0; x<trueImg.cols;x++) {
        int aCoded, aX, aY;
        int bCoded, bX, bY;
        aCoded =  trueImg (y,x);
        sobelVector (trueImg, x, y, aX, aY);
        bCoded = testImg (y,x);
        sobelVector (testImg, x, y, bX, bY);
        if (aCoded!=bCoded) {
            std::cout << "ERROR: " << trueName <<" at " << x << "," << y << " is " <<
            aCoded << " (" << aX << ", " << aY << ")" <<
            " " << testName << " is " <<
            bCoded << " (" << bX << ", " << bY << ")" <<
            std::endl;
            return false;
        }
    }
    return true;
}


void testCoreRoutines ()
{
    // We make a 3*10 image to compute result (1,1)..(8,1) having
    // enough border for a fully defined sobel
    uchar testImg[3][10];
    for (int y=0; y<3; y++) for (int x=0; x<10; x++) testImg[y][x] = 11+x*x + x*y;
    Mat_<uchar> testImgOpenCv (3, 10, &testImg[0][0]);
    ushort result[8], resultFast[8];
    for (int x=0; x<8; x++) result[x] = resultFast [x]  = sobelCode (0,0);
    
    // Compute slow
    Mat_<ushort>resultImg(testImgOpenCv.size());
    resultImg.setTo(sobelCode(0,0));
    for (int x=0; x<8; x++) { 
        sobel1Px(resultImg, testImgOpenCv, x+1, 1);
        result[x] = resultImg (1, x+1);
    }
    // Compute fast
    for (int x=0; x<8; x++) sobel1PxFast(&resultFast[x], &testImg[1][x+1], 10);
    
    
    // Compare
    printf ("x analytical sobel1Px sobel1PxFast\n");
    bool passed = true;
    for (int x=0; x<8; x++) {
        ushort analytical = sobelCode(3+2*x, 1+x);
        printf ("%1d (%3d,%3d) (%3d,%3d) (%3d,%3d) \n",
                x,
                sobelXUncode(analytical   ), sobelYUncode(analytical   ),
                sobelXUncode(result    [x]), sobelYUncode(result    [x]),
                sobelXUncode(resultFast[x]), sobelYUncode(resultFast[x])
                );
        if (result[x]!=analytical) passed=false;
        if (resultFast[x]!=analytical) passed=false;
    }
    if (passed) printf("testCoreRoutines PASSED\n");
    else printf("testCoreRoutines FAILED\n");
}



void detectEdges (const std::vector<std::string>& images, bool doSave)
{
    if (images.size()==0) return;
    
    // show as a window in the GUI
    namedWindow ("Tracking a flying ball", CV_WINDOW_AUTOSIZE);
    
    testCoreRoutines();
	
    for(int imageCtr=0; imageCtr<(int) images.size(); imageCtr++) {
        std::string fname = images[imageCtr];
        Mat_<uchar> srcImg = imread (fname, 0); // load image as greyscale
        if (srcImg.empty()) {
            std::cerr << "Could not find " << fname << std::endl;
            continue;
        }
        
        Mat overlayImg      = imread (fname.c_str(), 1); // load image as RGB for overlaying lines
        Mat_<ushort> sobelImg1 (srcImg.size());
        Mat_<ushort> sobelImg2 (srcImg.size());
        Mat_<ushort> sobelImg3 (srcImg.size());
        Mat sobelImgDisplay = overlayImg.clone();
        
        int64 time[4];

        time[0] = cvGetTickCount();
        sobel (sobelImg1, srcImg);
        time[1] = cvGetTickCount();
        sobelFast (sobelImg2, srcImg);
        time[2] = cvGetTickCount();
        sobelFastOpenMP (sobelImg3, srcImg);
        time[3] = cvGetTickCount();
        double tms[4];
        for (int i=0;i<3;i++) tms[i] = 0.001*(time[i+1]-time[i])/cvGetTickFrequency();
        
        compareImages (sobelImg1, sobelImg2, "sobel", "sobelFast");
        compareImages (sobelImg1, sobelImg3, "sobel", "sobelFastOpenMP");
        
        int nCores = 1;
#ifndef NOOPENMP
        nCores = omp_get_max_threads();
#endif
        std::cout << "Computation time ";
        std::cout << "slow: "  << tms[0] << "ms   (" << tms[0]/tms[1] << "x)  "
        << "fast:"   << tms[1] << "ms   (" << tms[1]/tms[2] << "x)  "
        << "oMP ("<< nCores << " cores):" << tms[2] << "ms"
        << std::endl;
        
        imshow ("Tracking a flying ball", overlayImg);
        while ((cvWaitKey (0) & 0xffff)!=' ');
        
        paintSobelImg (sobelImgDisplay, sobelImg2);
        imshow ("Tracking a flying ball", sobelImgDisplay);
        while ((cvWaitKey (0) & 0xffff)!=' ');
        
        if (doSave) {
            std::string rname = resultName (fname, ".sobel");
            imwrite (rname, sobelImgDisplay);   // save the resulting image
            std::cout << "Saving " << rname << std::endl;
        }  
    };
}
