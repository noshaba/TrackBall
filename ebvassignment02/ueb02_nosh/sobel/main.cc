#include <vector>
#include <string>
#include <iostream>


#if !defined(MAIN_NR)
#pragma message ("If this does not compile please define MAIN_NR")
#define MAIN_NR 1
#endif

#ifdef WIN32
#include <time.h>
#endif
  


#if MAIN_NR>=1
#include "sobelgui.h"
#endif
#if MAIN_NR==2 || MAIN_NR==5
#include "hcgui.h"
#endif
#if MAIN_NR>=3
#include "hlgui.h"
#endif
#if MAIN_NR>=4
#include "calGui.h"
#endif
#if MAIN_NR>=5
#include "trackGui.h"
#endif

void notImplemented ()
{
  std::cerr << "Not implemented" << std::endl;
  abort ();  
}


int main (int argc, char** argv)
{

  if (argc<2) {
#if MAIN_NR>=5
    std::cout << "ballgame [-randomseed] [-save] [<image>]           for full solution" << std::endl;    
#endif
#if MAIN_NR>=1 
    std::cout << "ballgame [-randomseed] [-save] -edges [<image>]    for edge detection" << std::endl;    
#endif
#if MAIN_NR==2 || MAIN_NR==5
    std::cout << "ballgame [-randomseed] [-save] -circles [<image>]  to find circles" << std::endl;
#endif
#if MAIN_NR>=3
    std::cout << "ballgame [-randomseed] [-save] -lines [<image>]    to find lines" << std::endl;
#endif
#if MAIN_NR>=4
    std::cout << "ballgame [-randomseed] [-save] -calibrate <image>  to calibrate the camera" << std::endl;
#endif
#if MAIN_NR>=5
    std::cout << "ballgame [-randomseed] [-save] -track [<image>]    to track the ball with hardcoded calibration" << std::endl;
#endif
    return 1;
  }
  std::vector<std::string> fname;
  bool doSave = false;
  enum {FULL, EDGES, CIRCLES, LINES, CALIBRATE, TRACK} mode = FULL;
  
  for (int i=1; i<argc; i++) 
    if (std::string(argv[i])=="-save") doSave = true;
    else if (std::string(argv[i])==std::string("-randomseed")) srand((unsigned int)time(NULL));  
    else if (std::string(argv[i])==std::string("-edges")) mode = EDGES;  
    else if (std::string(argv[i])==std::string("-circles")) mode = CIRCLES;
    else if (std::string(argv[i])==std::string("-lines")) mode = LINES;
    else if (std::string(argv[i])==std::string("-calibrate")) mode = CALIBRATE;
    else if (std::string(argv[i])==std::string("-track")) mode = TRACK;  
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
#if MAIN_NR>=1
    detectEdges (fname, doSave);    
#endif
  }  
  else if (mode==CIRCLES) {
#if MAIN_NR==2 || MAIN_NR==5
    trackBall (fname, doSave);
#else
    notImplemented();
#endif
  }  
  else if (mode==LINES) {
#if MAIN_NR>=3
    findLines (fname, doSave);
#else
    notImplemented();
#endif
  }  
  else if (mode==CALIBRATE) {
#if MAIN_NR>=4
    calibrate (fname[0], doSave);
#else
    notImplemented();
#endif     
  }  
  else if (mode==TRACK) {
#if MAIN_NR>=5
    Transform cameraInWorld( 0.9182917966, -0.0521184761,  0.3924587120, -0.6010138133,
                            -0.3957171197, -0.0903554395,  0.9139167663, -4.0805172618,
                            -0.0121711697, -0.9945449004, -0.1035968322,  1.6951552036,
                             0,             0,             0,             1);
    CameraCalibration camera;
    camera.cameraInWorld = cameraInWorld;
    camera.f = 765.950525;
    camera.width = 640;
    camera.height = 480;
    camera.centerX = camera.width / 2;
    camera.centerY = camera.height / 2;   
    trackBall3D (NULL, fname, doSave, camera);
#else
    notImplemented();
#endif
  }
  else if (mode==FULL) {
#if MAIN_NR>=5
    CameraCalibration camera;
    const char* title = "Tracking a flying ball.";
    cvNamedWindow (title, CV_WINDOW_AUTOSIZE);
    calibrateOneShot (title, camera, fname.front(), doSave);
    trackBall3D (title, fname, doSave, camera);
#else
    notImplemented();
#endif
  }  
  else {
    std::cout << "Illegal command " << std::endl;
    return 1;    
  }
  return 0;  
}
