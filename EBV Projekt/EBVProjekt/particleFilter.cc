#include "particleFilter.h"
#include <math.h>
#include <assert.h>

ParticleFilter::Parameters::Parameters ()
// TODO: choose good parameter (1P)
:ballRadius(0.02), sigmaVelocity(0), sigmaImage(0), deltaT(0), waitUntilSecondObservation(0), nrOfParticles(100)
{
  g[0] = 0;
  g[1] = 0;  
  g[2] = -9.81;
  g[3] = 0;
}

double ParticleFilter::randomUniform () 
{
  return ((double) rand())/RAND_MAX;  
}


double ParticleFilter::randomGaussian ()
{
  double u = randomUniform(), v = randomUniform();
  if (u==0) u=0.5/RAND_MAX;
  return sqrt(-2*log(u))*cos(2*M_PI*v);  
}

void ParticleFilter::Particle::observeInit (double x, double y, double r)
{
    // TODO: implement (2P)
	double
		pCenterX = filter->camera.centerX,
		pCenterY = filter->camera.centerY,
		alpha = filter->camera.alpha,
		Z = alpha * filter->param.ballRadius / r;

	double iX = pCenterX + alpha / Z * x;
	double iY = pCenterY + alpha / Z * y;

	double xInCamera = Z / alpha * (iX + randomGaussian() - pCenterX);
	double yInCamera = Z / alpha * (iY + randomGaussian() - pCenterY);

	if (filter->camera.generate(position, xInCamera, yInCamera, r, filter->param.ballRadius)){
		state = POSITIONDEFINED;
	}
}


void ParticleFilter::Particle::observeRegular (double x, double y, double r)
{
    // TODO: implement (1P)
}



void ParticleFilter::Particle::observe (double x, double y, double r)
{
    if (state==FULLDEFINED) observeRegular(x, y, r);
    else observeInit (x, y, r);
}


void ParticleFilter::dynamic ()
{
    // TODO: implement (included with Particle::dynamic)
}


void ParticleFilter::Particle::dynamic (double deltaT)
{
  // TODO: implement (2P)
}


void ParticleFilter::observe (double x, double y, double r)
{
  // TODO: implement (included with Particle::observe)
	for (int i = 0; i < particle.size(); ++i)
		particle[i].observe(x, y, r);
}


void ParticleFilter::createSamples (int nrOfParticles)
{
    // TODO: implement (included with ParticleFilter::Parameters::Parameters)
	double weight = 1.0 / nrOfParticles;
	for (int i = 0; i < nrOfParticles; ++i)
		particle.push_back(Particle(this, weight));
}


void ParticleFilter::resample (int nrOfParticles)
{
    // TODO: implement (1P)
}

