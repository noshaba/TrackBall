#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "central.h"
#include <opencv2/opencv.hpp>

//! A single line in Hesse normal form (without specified endpoints)
/*! It is defined as all points \c (x,y) for which 
  \c x*cos(angle) + y*sin(angle) - dist == 0
  Output of the overall line Hough algorithm.
*/
class Line 
{
 public:
  //! angle of the lines normal vector (see equation)
  double angle;
  
  //! signed distance in pixel to (0,0)
  /*! Not to the image center as in the Hough accumulator. (see equation)
   */
  double dist;
  
  //! Hough value for this line
  int value;    
  
  //! Empty line
  Line () :angle(0), dist(0), value(0) {}    
    
    //! Std constructor
    Line (double angle, double dist, int value)
      :angle(angle), dist(dist), value(value)
      {}  

      //! Line through two points
      Line (double x0, double y0, double x1, double y1)
	{
	  angle = atan2 (x1-x0, y0-y1);
	  dist  = cos(angle)*x0 + sin(angle)*y0;
	}      
      
      //! Returns the distance from \c (x,y) to the line
      /*! The distance is without sign. */
      double distance (double x, double y) const
      {
	return fabs(cos(angle)*x + sin(angle)*y - dist);
      }

      //! Computes the intersection of two lines
      /*! The intersection is returned in \c x,y. If the lines
          are equal or parallel, \c false is returned.
      */
      bool intersect (const Line& line2, double& x, double& y) const;

      //! Computes a measure of how different two lines are
      /*! In the measure, a difference in \c .angel of \c angleSigma is weighted
	  as 1 and \c .dist of \c distSigma is also weighted as 1.

	  The difference in angle takes both 2*M_PI peridodicity and the
	  "M_PI with negated distance" ambiguity into account.
      */
      double difference (const Line& line2, double angleSigma, double distSigma) const;      

      //! Returns the distance between two angles, taking 2*M_PI periodicity into account
      static double angularDistance (double angleA, double angleB);  

      //! exact equality
      bool operator == (const Line& line2) const
      {
	return angle==line2.angle && dist==line2.dist;
      }      
};

using namespace cv;

//! A transformation between two reference frames
/*! encoded by a homogenous matrix. Line 3 of this
 matrix is always 1 0 0 0). To avoid getting confused
 always view a Transform A2B as a mapping between
 reference frame A and B such that for a vector v
 expressed in A coordinates B*v is the corresponding
 vector expressed in B coordinates. Always name
 transforms as A2B with a frame name A and B.
 */
typedef Matx<double, 4, 4> Transform;

//! A projective mapping from homogenous 2D to homogenous 2D
typedef Matx<double, 3, 3> Homography;

//! A homogenous vector
typedef Vec<double, 4>     VVector;


//! Computes the inverse of matrix A.
/*! The routine needs only to handle those special matrices describing
    rigid body transformations.
    Caution: result can be identical to A
*/
void inverse (Transform& result, const Transform& A);

//! Moves the camera frame by '(x,y,z)' in camera coordinates
void translate (Transform& camera2World, double x, double y, double z);

//! Pitches (german Nicken) the camera by angle
void pitch (Transform& camera2World, double alpha);

//! Rolls (german rollen) the camera by angle
void roll (Transform& camera2World, double alpha);

//! Yaws (german gieren) the camera frame by angle 
void yaw (Transform& camera2World, double alpha);

//! Makes a transform orthonormal
void normalize (Transform& T);


//! A line in 3D space
/*! Represented by a point and a direction
 */
class Line3D 
{
 public:
  //! Vector of an arbitrary point on the 3D line
  VVector p0;

  //! Vector of the direction of the 3D line
  VVector pD;

  //! 0 constructor (not a valid line!)
  Line3D ()
    {
      p0[0] = p0[1] = p0[2] = p0[3] = 0;
      pD[0] = pD[1] = pD[2] = pD[3] = 0;
    }
  
  //! Std. constructor
  Line3D (const VVector& p0, const VVector& pD)
    {
      this->p0[0] = p0[0];
      this->p0[1] = p0[1];
      this->p0[2] = p0[2];
      this->p0[3] = p0[3];

      this->pD[0] = pD[0];
      this->pD[1] = pD[1];
      this->pD[2] = pD[2];
      this->pD[3] = pD[3];      
    }		

  //! Constructor with individual coordinates
  Line3D (double x0, double y0, double z0, double xD, double yD, double zD)
    {
      p0[0] = x0;
      p0[1] = y0;
      p0[2] = z0;
      p0[3] = 1;
      pD[0] = xD;
      pD[1] = yD;
      pD[2] = 0;
      pD[3] = 0;
    }
  

  //! Returns a parallel to the x axis (with given y)
  static Line3D xParallel (double y)
    {return Line3D (0, y, 0, 1, 0, 0);}

  //! Returns a parallel to the x axis (with given X)
  static Line3D yParallel (double x)
    {return Line3D (x, 0, 0, 0, 1, 0);}
};
#endif
