#include <vector>
#include <string>
#include <iostream>


#if !defined(MAIN_NR)
#pragma message("If this does not compile please define MAIN_NR")
#define MAIN_NR 3
#endif

#ifdef WIN32
#include <time.h>
#endif
  


#if MAIN_NR>=1
#include "sobelgui.h"
#endif
#if MAIN_NR>=2
#include "hcgui.h"
#endif
#if MAIN_NR>=3
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
#if MAIN_NR>=2
    std::cout << "ballgame [-randomseed] [-save] -circles [<image>]  to find circles" << std::endl;
#endif
#if MAIN_NR>=3
    std::cout << "ballgame [-randomseed] [-save] [<image>]    to track the ball with a particle filter (hardcoded calibration)" << std::endl;
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
    notImplemented();
  }
  else if (mode==CALIBRATE) {
    notImplemented();
  }
  else if (mode==TRACK || mode==FULL) {
#if MAIN_NR>=3
    CameraCalibration camera;
    CameraCalibration::ourCamera(camera);
    trackBall3D (NULL, fname, doSave, camera);
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
