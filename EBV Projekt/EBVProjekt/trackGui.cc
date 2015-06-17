#include "hcgui.h"
#include "particleFilter.h"
#include <iostream>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/core_c.h>

void testBallProjection ()
{
    CameraCalibration cal;
    CameraCalibration::example (cal);
    
    VVector p(2, 2.7, 0, 1);
    double rWorld=0.1, xImage, yImage, rImage;
    if (cal.project (xImage, yImage, rImage, p, rWorld)!=2) {
        cerr << "bug in project (test point is wrongly reported outside field of view" << endl;
        return;
    }
    VVector delta;
    delta[0] = p[0] - cal.cameraInWorld(0, 3);
    delta[1] = p[1] - cal.cameraInWorld(1, 3);
    delta[2] = p[2] - cal.cameraInWorld(2, 3);
    double dist = sqrt(delta[0]*delta[0] + delta[1]*delta[1] + delta[2]*delta[2]);
    double angle = atan(rWorld/dist);
    double rApprox = cal.alpha*angle;
    double factor = rApprox/rImage;
    if (!(0.8<=factor && factor<=1.2)) {
        cerr << "bug in project: radius is " << rImage << " but should be " << rApprox << "+/-20% but is not" << endl;
        return;
    }
    for (int ctr=0; ctr<100; ctr++) {
        VVector p2, delta;
        delta[0] = 2*ParticleFilter::randomUniform()-1;
        delta[1] = 2*ParticleFilter::randomUniform()-1;
        delta[2] = 2*ParticleFilter::randomUniform()-1;
        delta[3] = 0;
        double len = sqrt(delta[0]*delta[0]+delta[1]*delta[1]+delta[2]*delta[2]);
        if (len==0) continue;
        delta[0] *= 0.9*rWorld/len;
        delta[1] *= 0.9*rWorld/len;
        delta[2] *= 0.9*rWorld/len;
        
        p2[0] = p[0]+delta[0];
        p2[1] = p[1]+delta[1];
        p2[2] = p[2]+delta[2];
        p2[3] = 1;
        
        double xImage2, yImage2;
        if (cal.project (xImage2, yImage2, p2)!=2) {
            cerr << "bug in project (test point is wrongly reported outside field of view" << endl;
            return;
        }
        
        double distImage = sqrt((xImage-xImage2)*(xImage-xImage2) + (yImage-yImage2)*(yImage-yImage2));
        if (distImage>rImage) {
            cerr << "Error in project: A point inside the ball is projected to outside the ball projection ("
            << distImage << ">" << rImage
            << endl;
            return;
        }
    }
}

// paint a checkboard at by transforming world points into the image
// using 'camera.project' and display them onto 'overlay'.
void paintCheckerboard (Mat& overlay, const CameraCalibration& camera, int gridExtension, double tilingLength, bool withCoordinates)
{
    // Crosses
    for (int xx=-gridExtension; xx<=gridExtension; xx++)
        for (int yy=-gridExtension; yy<=gridExtension; yy++) {
            VVector pInWorld;
            pInWorld[0] = xx*tilingLength;
            pInWorld[1] = yy*tilingLength;
            pInWorld[2] = 0;
            pInWorld[3] = 1;
            
            double x, y;
            int isInsideImage;
            isInsideImage = camera.project (x, y, pInWorld);
            if (isInsideImage==2) {
                paintCross (overlay, (int) x, (int) y, CV_RGB(0,255,0));
                if (withCoordinates) {
                    char txt[100];
                    sprintf_s(txt, "%d/%d", xx, yy);
                    putText (overlay, txt , cvPoint((int) x, (int) y), FONT_HERSHEY_PLAIN, 2, CV_RGB(255,255,0));
                }
            }
        }
}



// Paints the set of particles. For each particle the current position
// is shown in red and if the particle's velocity is defined the position,
// where it will hit the floor is shown in blue.
void paintParticleFilter (Mat& overlay, const ParticleFilter& pF)
{
    int sgnZ = +1;
    if (pF.camera.cameraInWorld(2, 3)<0) sgnZ = -1;
    
    for (int i=0; i<(int) pF.particle.size(); i++) {
        ParticleFilter::Particle p = pF.particle[i];
        if (p.state>=ParticleFilter::Particle::POSITIONDEFINED) {
            double x, y;
            if (pF.camera.project (x, y, p.position)>0 && sgnZ*p.position[2]>=0) {
                paintCross (overlay, (int) x, (int) y, CV_RGB(255, 0, 0));
                Point lastP ((int) x, (int) y);
                if (p.state>=ParticleFilter::Particle::FULLDEFINED) {
                    int ctr=0;
                    while (sgnZ*(p.position[2]-pF.param.ballRadius)>0 && ctr<500) {
                        p.dynamic (0.005);
                        ctr++;
                        if (ctr%6==0 || sgnZ*(p.position[2]-pF.param.ballRadius)<0)
                            if (pF.camera.project (x, y, p.position)>0) {
                                Point p ((int) x, (int) y);
                                line (overlay, lastP, p, CV_RGB (0, 0, 255));
                                lastP = p;
                            }
                    }
                }
            }
        }
    }
}


void trackBall3D (const char* title, const vector<string>& images, bool doSave, const CameraCalibration& cal)
{
    testBallProjection();
    if (images.size()==0) return;
    
    // show as a window in the GUI
    if (title==NULL) {
        title = "Tracking a flying ball";
        namedWindow (title, CV_WINDOW_AUTOSIZE);
    }
    
    // Load the first image to get the image dimensions
    Mat_<uchar> srcImg = imread (images[0], 0);
    if (srcImg.empty()) {
        cerr << "Could not find " << images[0] << endl;
        return;
    }
    HoughCircle hc;
    hc.create (srcImg.cols, srcImg.rows);
    
    // for calculation of sobel differences between two consecutive images
    Mat_<ushort> sobelImgPrev (srcImg.size());
    sobelImgPrev.setTo(Scalar(0));
    
    ParticleFilter pf (cal);
    
    int imageCtr=0;
    while (imageCtr<(int) images.size()) {
        string fname = images[imageCtr];
        srcImg = imread (fname, 0); // load image as greyscale
        if (srcImg.empty()) cerr << "Could not find " << fname << endl;
        
        if (srcImg.cols!=hc.imgWidth || srcImg.rows!=hc.imgHeight) {
            cerr << "Image " << fname << " has wrong size." << endl;
            return;
        }
        Mat overlayImg = imread (fname, 1); // load image as RGB for overlaying lines
        Mat_<ushort> houghImg = hc.createHoughImage ();
        Mat_<ushort> sobelImg (srcImg.size());
        
        int64 time1, time2;
        
        time1 = getTickCount();
        
        sobelFastOpenMP (sobelImg, srcImg);
        vector<HoughCircle::Circle> circles;
        hc.hough (houghImg, sobelImgPrev, sobelImg);
        hc.extractFromHoughImage (circles, houghImg, sobelImg);
        sobelImg.copyTo(sobelImgPrev);
        
        pf.dynamic ();
        if (circles.size()==1 &&
            0<=circles.front().xC && circles.front().xC<srcImg.cols &&
            0<=circles.front().yC && circles.front().yC<srcImg.rows)
            pf.observe (circles.front().xC, circles.front().yC, circles.front().r);
        else if (imageCtr>0) cout << "Warning: " << circles.size() << "!=1 circles found, ignoring this image" << endl;
        pf.resample ();    
        
        time2 = cvGetTickCount();    
        
        
        cout << "Computation time ";
        cout << 0.001*(time2-time1)/cvGetTickFrequency() << "ms" << endl;
        
        paintCheckerboard (overlayImg, cal, 10, 0.66, false);
        paintCircles (overlayImg, circles);
        paintParticleFilter (overlayImg, pf);    
        
        imshow (title, overlayImg);
        while ((cvWaitKey (0) & 0xffff)!=' ');
        
        if (doSave) {
            string rname = resultName (fname, ".tracked");
            imwrite (rname, overlayImg);   // save the resulting image
            cout << "Saving " << rname << endl;
        }  
                
        imageCtr++;    
    };  
}

