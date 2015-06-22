#ifndef TRACKGUI_H
#define TRACKGUI_H

#include "central.h"
#include "cameraCalibration.h"

void trackBall3D (const char* title, const vector<string>& images, bool doSave, const CameraCalibration& cal);



#endif
