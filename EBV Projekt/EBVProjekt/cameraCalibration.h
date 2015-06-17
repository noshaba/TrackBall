#ifndef CAMERA_CALIBRATION
#define CAMERA_CALIBRATION

#include "central.h"
#include "geometry.h"
using namespace std;

//! Calibration information about a camera.
/*! Pose (where it is)
    focal length (zoom, wide angle)
    and information on the image format.
*/
class CameraCalibration 
{
public:
  //! Camera pose in world frame
  Transform cameraInWorld;

  //! Effective focal length
  double alpha; 

  //! Image center (i.e. image position of optical axis)
  double centerX, centerY;

  //! Image format (for clipping)
  int width, height;

  //! Returns, whether \c x,y lies inside the image
  bool isInside (double x, double y) const
  {return 0<=x && x<width && 0<=y && y<height;}  
    

  //! Computes the perspective projection (in pixel) of \c p
  /*! \c p is in world coordinates. The return value is 0 if the
      point is behind the camera, 1 if it is in front of the
      camera but outside the image region and 2 if it is inside
      the image.

      Some helper routines are found in geometry.h.
  */
  int project (double& x, double& y, const VVector& pInWorld) const;


  //! Project a sphere of \c radius at \c p into the image
  /*! Use the approximated formula for computing the resulting image circle
      centered at \c x,y with radius \c r.
      We need this function only for the 6th exercise.
  */
  int project (double& x, double& y, double& r, const VVector& pInWorld, double radius) const;

  //! Compute the world position of an image sphere.
  /*! The image spehere has world \c radius that is viewed by the camera
      at image position \c (x,y) with image radius \c r.
      This is the inverse problem solved by \c project. 

      The returns value specifies, if the generation was successful
      and should return false for illegal x,y,r.
  */
  bool generate (VVector& pInWorld, double x, double y, double r, double radius) const;


  static void example (CameraCalibration& camera)
  {
    Transform cameraInWorld(1,  0, 0,  1,
                            0,  0, 1, -2,
                            0, -1, 0,  1,
                            0,  0, 0,  1);
    camera.cameraInWorld = cameraInWorld;
    camera.width  = 640;
    camera.height = 480;
    camera.centerX = camera.width/2;
    camera.centerY = camera.height/2;
    camera.alpha   = 400;
  }

  static void ourCamera (CameraCalibration& camera)
  {
    Transform cameraInWorld( 0.9182917966, -0.0521184761,  0.3924587120, -0.6010138133,
                            -0.3957171197, -0.0903554395,  0.9139167663, -4.0805172618,
                            -0.0121711697, -0.9945449004, -0.1035968322,  1.6951552036,
                             0,             0,             0,             1);
    camera.cameraInWorld = cameraInWorld;
    camera.alpha = 765.950525;
    camera.width = 640;
    camera.height = 480;
    camera.centerX = camera.width / 2;
    camera.centerY = camera.height / 2;   
  }

};
#endif
