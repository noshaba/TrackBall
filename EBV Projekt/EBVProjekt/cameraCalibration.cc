#include "cameraCalibration.h"
#include <math.h>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>



int CameraCalibration::project (double& x, double& y, const VVector& pInWorld) const
{
  // TODO: implement (1P)
	Transform inverseCameraInWorld;
	inverse(inverseCameraInWorld, cameraInWorld);
	VVector pInCamera = inverseCameraInWorld * pInWorld;

	// check if behind camera
	int inside = pInCamera[2] > 0 ? 1 : 0;

	// no K_1 or K_2?!
	// what to do if z == 0?! division by 0, is this even necessary since point is behind camera?
	x = centerX + alpha * pInCamera[0] / pInCamera[2];
	y = centerY + alpha * pInCamera[1] / pInCamera[2];

	// check if inside image
	inside = isInside(x, y) ? 2 : inside;

	return inside;
}


int CameraCalibration::project (double& x, double& y, double& r, const VVector& pInWorld, double radius) const
{
  // TODO: implement (1P)
	int inside = project(x,y,pInWorld);

	double x_r, y_r;
	project(x_r, y_r, pInWorld + VVector(radius,0,0,0));
	x_r -= x;
	y_r -= y;

	r = sqrt(x_r*x_r+y_r*y_r);

	return inside;
}


bool CameraCalibration::generate (VVector& pInWorld, double x, double y, double r, double radius) const
{
// TODO: implement (1P)
 return false;
}


