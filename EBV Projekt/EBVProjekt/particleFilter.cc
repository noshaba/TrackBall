#include "particleFilter.h"
#include <math.h>
#include <assert.h>

ParticleFilter::Parameters::Parameters ()
// TODO: choose good parameter (1P)
:ballRadius(0.1), sigmaVelocity(0.5), sigmaImage(2), deltaT(1.0 / 25), waitUntilSecondObservation(3.0 / 25), nrOfParticles(200)
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
	// add noise
	x += sigma * randomGaussian();
	y += sigma * randomGaussian();
	r += sigma * randomGaussian();
	if (filter->camera.generate(p, x, y, r, filter->param.ballRadius)) {
		if (state == POSITIONDEFINED) {
			double dT = time - timeOfLastObservation;
			// determine velocity after waitUntilSecondObservation has passed
			if (dT > filter->param.waitUntilSecondObservation) {
				// acceleration added to initial velocity for analytical approach
				velocity = (p - position) / dT + filter->param.g * dT * .5;
				position = p;
				state = FULLDEFINED;
				timeOfLastObservation = time;
			}
		}
		else {
			// determine initial position of particle
			position = p;
			state = POSITIONDEFINED;
			timeOfLastObservation = time;
		}
	}
	// for a better result, particles outside of field of view can be already winthdrawn in the initialization
	else weight = 0;
	if (position[2] < 0) weight = 0;
}


void ParticleFilter::Particle::observeRegular (double x, double y, double r)
{
    // TODO: implement (1P)
	double dx, dy, dr, sigma = filter->param.sigmaImage;
	// ignore particle if behind camera
	if (!filter->camera.project(dx, dy, dr, position, filter->param.ballRadius)) 
		weight = 0;
	else {
		// weight particle
		dx = x - dx;
		dy = y - dy;
		dr = r - dr;
		weight *= exp(-(dx*dx + dy*dy + dr*dr) / (2 * sigma * sigma));
	}
	timeOfLastObservation = time;
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
		VVector n = { randomGaussian(), randomGaussian(), randomGaussian(), 0};
		// analytical approach
		position += deltaT * velocity + deltaT * deltaT * filter->param.g * .5;
		velocity += deltaT * filter->param.g + filter->param.sigmaVelocity * sqrt(deltaT) * n;
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
	// 'j' and 'weightUpToJ' just for solution in lecture
	double totalWeight = 0, weightUpToJ = 0;
	int j = -1;
	vector<Particle> pNew;

	// calculate total weight
	for (int i = 0; i < particle.size(); ++i)
		totalWeight += particle[i].weight;

	double normWeight = totalWeight / nrOfParticles, unitWeight = 1.0 / nrOfParticles;
	// initial "wheel of fortune" pointer
	double weightChosen = randomUniform() * normWeight;

	for (int i = 0; i < nrOfParticles; i++) {

		// solution from lecture creates an infinite loop because the Gaussian distribution becomes 0 at some points
		// where the particle is too far from the actual cirle (numerical underflow)

		/*
			while (weightChosen >= weightUpToJ) {
				j++;
				weightUpToJ += particle[i].weight;
			}
			pNew.push_back(particle[j]);
			pNew.back().weight = unitWeight;
			weightChosen += normWeight;
		*/

		// current "wheel of fortune" pointer
		weightChosen -= particle[i].weight;
		// add normWeight to the current pointer until the wanted wight is archieved
		while (weightChosen <= 0) {
			pNew.push_back(particle[i]);
			pNew.back().weight = unitWeight;
			// next "wheel of fortune" pointer
			weightChosen += normWeight;
		}
	}
	// make sure sizes are equal
	assert("Resampled particles must have the size of nrOfParticles" && pNew.size() == nrOfParticles);
	particle = pNew;
}

