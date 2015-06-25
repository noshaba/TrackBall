#include "particleFilter.h"
#include <math.h>
#include <assert.h>

ParticleFilter::Parameters::Parameters ()
// TODO: choose good parameter (1P)
:ballRadius(0.1), sigmaVelocity(0.5), sigmaImage(2), deltaT(1.0 / 25), waitUntilSecondObservation(0.125), nrOfParticles(200)
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
	double sigma = filter->param.sigmaImage;
	VVector p;
	// TODP: multiply rndGauss with sigma or not?
	x += randomGaussian();
	y += randomGaussian();
	r += randomGaussian();
	if (filter->camera.generate(p, x, y, r, filter->param.ballRadius)) {
		if (state == POSITIONDEFINED) {
			double dT = time - timeOfLastObservation;
			if (dT > filter->param.waitUntilSecondObservation) {
				velocity = (p - position) / dT + filter->param.g * dT * .5; // TODO: add acceleration or not?
				position = p;
				state = FULLDEFINED;
				timeOfLastObservation = time;
			}
		}
		else {
			position = p;
			state = POSITIONDEFINED;
			timeOfLastObservation = time;
		}
	}
	// is weighting allows in observe?
	else weight = 0;
	if (position[2] < 0) weight = 0;
}


void ParticleFilter::Particle::observeRegular (double x, double y, double r)
{
    // TODO: implement (1P)
	double dx, dy, dr, sigma = filter->param.sigmaImage;
	if (!filter->camera.project(dx, dy, dr, position, filter->param.ballRadius)) weight = 0;
	dx = x - dx;
	dy = y - dy;
	dr = r - dr;
	weight *= exp(-(dx*dx + dy*dy + dr*dr) / (2*sigma*sigma));
	timeOfLastObservation = time;
	if (position[2] < 0) weight = 0; // is this allowed in observe?
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
		particle[i].dynamic(param.deltaT);
}


void ParticleFilter::Particle::dynamic (double deltaT)
{
	// TODO: implement (2P)
	if (state == FULLDEFINED) {
		VVector ones = { 1, 1, 1, 0 };
		VVector n = { randomGaussian(), randomGaussian(), randomGaussian(), 0};
		position += (deltaT * velocity + deltaT * deltaT * filter->param.g * .5).mul(ones); // mul = element wise multiplication
		// isn't vectorized acceleration better than an acceleration scalar (in lectures) 
		// since no check for length of vector is needed??
		velocity += (deltaT * filter->param.g + filter->param.sigmaVelocity * sqrt(deltaT) * n).mul(ones);
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

