#ifndef PARTICLE_FILTER_H
#define PARTICLE_FILTER_H

#include "cameraCalibration.h"
#include <vector>
using namespace std;



//! Class encapsulating a particle filter for tracking a flying ball
//! based on observations of its image position and radius.    
class ParticleFilter 
{
public:
  //! The parameters used by this algorithm
  class Parameters 
  {
  public:
    //! radius of the ball thrown [meter]
    double ballRadius;  

    //! Gravitation [meter/s^2]
    VVector g;

    //! Root of mean square disturbance of the ball's velocity during one second of flight
    /*! Unit [m/s^{3/2}] */
    double sigmaVelocity;  

    //! Root of mean square error of the ball's image position and radius
    double sigmaImage;  

    //! Time between two images
    double deltaT;  

    //! How long to wait until second observation
    /*! After the first observation of the ball wait for at least
        \c waitUntilSecondObservation and ignore observations before
        integrating the second ball observation
    */
    double waitUntilSecondObservation;    

    //! Nr of particles used by the filter
    int nrOfParticles;    
    
    //! The default parameters actually applied
    Parameters ();    
  };

  //! The parameters actually used by the algorithm
  Parameters param;  
  
  //! A single particle representing one possible position and velocity of the ball.
  class Particle 
  {
  public:
    //! Pointer to the overall filter
    const ParticleFilter* filter;

    //! Probability weight of this particle
    /*! representing how likely it is, that this particle is "true". */
    double weight;

    //! state for the initialization phase (until 2nd ball is observed)    
    /*! UNDEFINED: no ball was observed, position, velocity are invalid
        POSITIONDEFINED: 1 ball was observed, position valid, velocity invalid
        FULLDEFINED: 2 balls were observed, position and velocity valid
    */
    enum State {UNDEFINED, POSITIONDEFINED, FULLDEFINED};

    //! The current state
    State state;    

    //! Position of the ball [m] in world coordinates. 
    /*! The coordinate system is defined
        by the floor grid (X, Y) plane with Z pointing upward as computed by
        the \c Ransac calibration.
    */
    VVector position;
	VVector initPosition;
	VVector initVelocity;

    //! Velocity of the ball [m/s] in world coordinates.
    VVector velocity;

    //! Current time (starting at 0 at the first image)
    double time;

    //! time when the ball was observed the last time
    /*! This is used to initialize the velocity in state \c POSITIONDEFINED.
     */
    double timeOfLastObservation;    

    //! Default constructor
    Particle (const ParticleFilter* pf, double weight)
      :filter (pf), weight(weight), state(UNDEFINED), time(0), timeOfLastObservation(0)
    {}    

    //! Integrate an observation of the ball
    /*! A ball has been seen with image \c x, y and radius \c r.
        \c filter->param.sigmaImage is the rms error of image processing [pixel].
        Calls \c observeInit or \c observeRegular respectively.
    */
    void observe (double x, double y, double r);

    //! Integrate an observation of the ball during initialization
    /*
        Initialization means (\c state==UNDEFIED || ==POSITIONSDEFINED)
     Note, if the ball has been observed only one time we have to wait
     for at least \c param.waitUntilSecondObservation before integrating
     the second observation.
     */
    void observeInit (double x, double y, double r);

    //! Integrate an observation of the ball once the particle is fully initialized
    /*! Requires \c state==FULLDEFINED */ 
    void observeRegular (double x, double y, double r);
  
  
    //! Time \c deltaT has passed. 
    /*! Update the particle accordingly. I.e.  move it according to
        the motion model and add to time.  Additionally noise of \c
        sqrt(deltaT)*filter->param.sigmaVelocity is added to the velocity.
        Since the noise term scales by \c sqrt(deltaT) the
        numbers correspond to the overall noise added after 1s of
        movement.

	Note, use the \c deltaT passed instead of \c
	param.deltaT. This functionality is needed for visualizing the
	blue trajectories in the GUI.
    */
    void dynamic (double deltaT);
  };  

  //! Camera calibration 
  /*! including camera pose and focal length
      used to project between world and image coordinates.
  */
  CameraCalibration camera;  


  
  //! Set of particles representing the distribution of the uncertain
  //! ball position and velocity.
  vector<Particle> particle;  

  //! Default constructor
  /*! \c camera is the camera calibration */
  ParticleFilter (CameraCalibration camera, const Parameters& param=Parameters())
    :param(param), camera(camera), particle()
    {
      createSamples (param.nrOfParticles);      
    };  

  //! A ball has been seen with image \c x,y and radius \c r.
  void observe (double x, double y, double r);
  
  //! Time \c deltaT has passed. Proceed to the next image and update the particles accordingly.
  void dynamic ();

  //! Initialize the particle filter with \c nrOfParticles particles
  //! of state \c UNDEFINED.
  void createSamples (int nrOfParticles);  
  
  //! Draw particles from the particle set, resetting their weight to 1.
  /*! This is done in a way that they are distributed according to the probability distrubtion
      represented by 'particle[?].weight'.
  */
  void resample (int nrOfParticles);  

  //! Resample without changing the number of particles
  void resample () {resample (particle.size());}
  

  //! Returns a uniformly [0..1] distributed random number
  static double randomUniform ();  

  //! Returns a mean=0, sigma=1 Gaussian distributed random number
  static double randomGaussian ();  
};

#endif
