#include "cameraCalibration.h"
#include <math.h>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>



int CameraCalibration::project (double& x, double& y, const VVector& pInWorld) const
{
  // TODO: implement (1P)
	Transform worldInCamera;
	inverse(worldInCamera, cameraInWorld);
	VVector pInCamera = worldInCamera * pInWorld;

	// check if behind camera
	int inside = pInCamera[2] > 0 ? 1 : 0;

	x = centerX + alpha * pInCamera[0] / pInCamera[2];
	y = centerY + alpha * pInCamera[1] / pInCamera[2];

	// check if inside image
	inside = isInside(x, y) ? 2 : inside;

	return inside;
}


int CameraCalibration::project (double& x, double& y, double& r, const VVector& pInWorld, double radius) const
{
  // TODO: implement (1P)
	Transform worldInCamera;
	inverse(worldInCamera, cameraInWorld);
	VVector pInCamera = worldInCamera * pInWorld;

	// check if behind camera
	int inside = pInCamera[2] > 0 ? 1 : 0;

	x = centerX + alpha * pInCamera[0] / pInCamera[2];
	y = centerY + alpha * pInCamera[1] / pInCamera[2];
	r = alpha * radius / pInCamera[2];

	// check if inside image
	inside = isInside(x, y) ? 2 : inside;

	return inside;
}


bool CameraCalibration::generate (VVector& pInWorld, double x, double y, double r, double radius) const
{
// TODO: implement (1P)
	if(r <= 0) return false;

	VVector pInCamera;
	pInCamera[3] = 1;
	pInCamera[2] = alpha * radius / r;
	pInCamera[1] = (y - centerY) * pInCamera[2] / alpha;
	pInCamera[0] = (x - centerX) * pInCamera[2] / alpha;

	pInWorld = cameraInWorld * pInCamera;

	// check if input of generate is equal to output of project
	double xImage, yImage, rImage;
	project(xImage, yImage, rImage, pInWorld, radius);
	assert("project(generate(INPUT)) must be equal to INPUT!" && 
		fabs(x - xImage) + fabs(y - yImage) + fabs(r - rImage) < 0.1);
	return true;
}


