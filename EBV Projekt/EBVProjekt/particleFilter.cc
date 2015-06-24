#include "particleFilter.h"
#include <math.h>
#include <assert.h>

ParticleFilter::Parameters::Parameters ()
// TODO: choose good parameter (1P)
:ballRadius(0.02), sigmaVelocity(0.5), sigmaImage(0), deltaT(1.0 / 25), waitUntilSecondObservation(0.1), nrOfParticles(100)
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
		radius = filter->param.ballRadius,
		Z = alpha * radius / r;

	double iX = pCenterX + alpha / Z * x;
	double iY = pCenterY + alpha / Z * y;

	double pX = Z / alpha * (iX + randomGaussian() - pCenterX);
	double pY = Z / alpha * (iY + randomGaussian() - pCenterY);

	if (filter->camera.generate(position, pX, pY, r, radius)) {
		if (state == POSITIONDEFINED){
			double dT = time - timeOfLastObservation;
			if (dT > filter->param.waitUntilSecondObservation) {
				double vX = 1.0 / dT * (pX - Z / alpha * (iX + randomGaussian() - pCenterX));
				double vY = 1.0 / dT * (pY - Z / alpha * (iY + randomGaussian() - pCenterY));
				filter->camera.generate(velocity, vX, vY, r, radius);
				state = FULLDEFINED;
				timeOfLastObservation = time;
			}
		}
		else {
			state = POSITIONDEFINED;
			timeOfLastObservation = time;
		}
	}
	else {
		weight = 0;
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
	for (int i = 0; i < particle.size(); ++i) 
		// TODO: really param.deltaT?!
		particle[i].dynamic(param.deltaT);
}


void ParticleFilter::Particle::dynamic (double deltaT)
{
	// TODO: implement (2P)
	if (state == FULLDEFINED) {
		VVector ones = { 1, 1, 1, 0 };
		position += (deltaT * velocity + deltaT * deltaT * filter->param.g * .5).mul(ones); // mul = element wise multiplication
		velocity += (deltaT * filter->param.g + filter->param.sigmaVelocity * sqrt(deltaT) * randomGaussian() * ones).mul(ones);
	}
	time += deltaT;
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

