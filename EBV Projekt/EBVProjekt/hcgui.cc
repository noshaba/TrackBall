#include "hcgui.h"
#include "sobelgui.h"
#include <iostream>

void paintCircles (Mat& overlay, const vector<HoughCircle::Circle>& circles)
{
    for (int i=0; i<(int) circles.size(); i++) {
        Point center (circles[i].xC, circles[i].yC);
        if (circles[i].r>0) circle (overlay, center, circles[i].r, CV_RGB (255, 0, 0));
        else paintCross (overlay, circles[i].xC, circles[i].yC, CV_RGB(255, 0, 0));
    }
}

void printCircles (const vector<HoughCircle::Circle>& circles)
{
    for (int i=0; i<(int) circles.size(); i++) {
        cout << circles[i].xC << " " << circles[i].yC << " " << circles[i].r
        << "(value: " << circles[i].value << "/" << circles[i].valueR
        << ")" << endl;
    }
}


void trackBall (const vector<string>& images, bool doSave)
{
    if (images.size()==0) return;
    
    // show as a window in the GUI
    namedWindow ("Tracking a flying ball", CV_WINDOW_AUTOSIZE);
    
    // Load the first image to get the image dimensions
    Mat_<uchar> srcImg = imread(images[0], 0);
    if (srcImg.empty()) {
        cerr << "Could not find " << images[0] << endl;
        return;
    }
    HoughCircle hc;
    hc.create (srcImg.cols, srcImg.rows);
    
    if (hc.param.houghThreshold==0) cerr << "Please set HoughCircle::houghThreshold\n" << endl;
    if (hc.param.sobelThreshold==0) cerr << "Please set HoughCircle::sobelThreshold" << endl;
    
    // for calculation of sobel differences between two consecutive images
    Mat_<ushort> sobelImgPrev (srcImg.size());
    sobelImgPrev.setTo (sobelCode(0,0));
	
    int imageCtr=0;
    while (imageCtr<(int) images.size()) {
        string fname = images[imageCtr];
        srcImg = imread (fname, 0); // load image as greyscale
        if (srcImg.empty()) cerr << "Could not find " << fname << endl;
        
        if (srcImg.cols!=hc.imgWidth || srcImg.rows!=hc.imgHeight) {
            cerr << "Image " << fname << " has wrong size." << endl;
            //      return;
        }
        Mat overlayImg = imread (fname, 1); // load image as RGB for overlaying lines
        Mat_<ushort> houghImg;
        Mat_<ushort> sobelImg (srcImg.size());
        
        int64 time1, time2;
        
        time1 = getTickCount();
        
        sobelFastOpenMP (sobelImg, srcImg);
        vector<HoughCircle::Circle> circles;
        hc.findCircles (circles, houghImg, sobelImgPrev, sobelImg);
        
        time2 = getTickCount();
        
        sobelImg.copyTo(sobelImgPrev);
        
        cout << "Computation time ";
        cout << (time2-time1)/getTickFrequency() << "s" << endl;
        cout << circles.size() << " circles detected" << endl;
        
        printCircles (circles);
        paintCircles (overlayImg, circles);
        
        
        imshow ("Tracking a flying ball", overlayImg);
        while ((waitKey (0) & 0xffff)!=' ');
        
        if (!houghImg.empty()) {
            scaleHoughImage (houghImg, 2*hc.param.houghThreshold);
            imshow ("Tracking a flying ball", houghImg);
            while ((waitKey (0) & 0xffff)!=' ');
        }
        
        if (doSave) {
            string rname = resultName (fname, ".circles");
            imwrite (rname, overlayImg);   // save the resulting image
            cout << "Saving " << rname << endl;
        }  
        
        imageCtr++;
    };
}
