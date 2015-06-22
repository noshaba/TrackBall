#include "geometry.h"

void inverse(Transform& result, const Transform& A)
{
  Matx<double, 3, 3> rot = A.get_minor<3, 3>(0, 0);
  Matx<double, 3, 1> trans = A.get_minor<3, 1>(0, 3);

  rot = rot.t();
  trans = -(rot * trans);

  // No set_minor method in Matx.
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
      result(i, j) = rot(i, j);
    result(i, 3) = trans(i, 0);
    result(3, i) = 0.0;
  }
  result(3, 3) = 1.0f;
}


void translate (Transform& camera2World, double x, double y, double z)
{
  Transform new2Camera(1, 0, 0, x,
                       0, 1, 0, y,
                       0, 0, 1, z,
                       0, 0, 0, 1);
  camera2World = camera2World * new2Camera;
}

void pitch (Transform& camera2World, double alpha)
{
  double c = cos(alpha);
  double s = sin(alpha);
  Transform new2Camera(1, 0,  0, 0,
                       0, c, -s, 0,
                       0, s,  c, 0,
                       0, 0,  0, 1);
  camera2World = camera2World * new2Camera;
}


void roll (Transform& camera2World, double alpha)
{
  double c = cos(alpha);
  double s = sin(alpha);
  Transform new2Camera(c, -s, 0, 0,
                       s,  c, 0, 0,
                       0,  0, 1, 0,
                       0,  0, 0, 1);
  camera2World = camera2World * new2Camera;
}


void yaw (Transform& camera2World, double alpha)
{
  double c = cos(alpha);
  double s = sin(alpha);
  Transform new2Camera( c, 0, s, 0,
                        0, 1, 0, 0,
                       -s, 0, c, 0,
                        0, 0, 0, 1);
  camera2World = camera2World * new2Camera;
}

bool Line::intersect (const Line& line2, double& x, double& y) const      
{
  // equation to be solved
  // (cos(line2.alpha)*line2.dist + lambda*-sin(line2.alpha))*cos(alpha)
  //+(sin(line2.alpha)*line2.dist + lambda* cos(line2.alpha))*sin(alpha)
  // -dist = 0
  
  // cast as ax+b=0
  double b = line2.dist*(cos(line2.angle)*cos(angle)+sin(line2.angle)*sin(angle))-dist;
  double a = -sin(line2.angle)*cos(angle) + cos(line2.angle)*sin(angle);
  if (fabs(a)>1E-5) {
    double lambda = -b/a;	  
    x = line2.dist*cos(line2.angle) + lambda*-sin(line2.angle);
    y = line2.dist*sin(line2.angle) + lambda* cos(line2.angle);
    assert ("intersection does not lie on both lines"
	    && distance(x,y)<1E-3 && line2.distance(x, y)<1E-3);	  
    return true;	  
  }
  else return false;
}      


void normalize (Transform& T)
{
  // length of colum 0 --> 1
  double f = 1/sqrt(T(0, 0)*T(0, 0) + T(1, 0)*T(1, 0) + T(2, 0)*T(2, 0));
  T(0, 0) *=f;
  T(1, 0) *=f;
  T(2, 0) *=f;
  
  // make column 0 and 1 orthogonal
  double c0c1 = T(0, 0)*T(0, 1) + T(1, 0)*T(1, 1) + T(2, 0)*T(2, 1);  
  T(0, 1) -= T(0, 0)*c0c1;
  T(1, 1) -= T(1, 0)*c0c1;
  T(2, 1) -= T(2, 0)*c0c1;
  
  // length of column 1 --> 1
  f = 1/sqrt(T(0, 1)*T(0, 1) + T(1, 1)*T(1, 1) + T(2, 1)*T(2, 1));
  T(0, 1) *= f;
  T(1, 1) *= f;
  T(2, 1) *= f;
  
  // column 2 cross product
  T(0, 2) = T(1, 0)*T(2, 1) - T(2, 0)*T(1, 1);  
  T(1, 2) = T(2, 0)*T(0, 1) - T(0, 0)*T(2, 1);  
  T(2, 2) = T(0, 0)*T(1, 1) - T(1, 0)*T(0, 1);    

  // lower row
  T(3, 0) = T(3, 1) = T(3, 2) = 0;
  T(3, 3) = 1;  
}

double Line::angularDistance (double angleA, double angleB)
{
  double delta = fabs(fmod(angleA-angleB, 2*M_PI));
  if (delta>M_PI) return 2*M_PI-delta;  
  else return delta;  
}


double Line::difference (const Line& line2, double angleSigma, double distSigma) const
{
  double d1D = (dist-line2.dist)/distSigma;
  double d1A = angularDistance (angle, line2.angle)/angleSigma;  
  double d1 = sqrt( d1D*d1D + d1A*d1A);
  
  double d2D = (dist+line2.dist)/distSigma;
  double d2A = angularDistance (angle, line2.angle+M_PI)/angleSigma;  
  double d2 = sqrt( d2D*d2D + d2A*d2A);
  if (d1<d2) return d1;
  else return d2;  
}


