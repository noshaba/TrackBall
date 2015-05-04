#include "include/Region.hpp"

void Region::eigenDecompositionSymmetric(const Matrix2x2& a, double& phi, double& l0, double& l1) {
	double ll0, ll1;
	double denom = a[1][1] - a[0][0], nom = -2 * a[1][0];
	if (denom != 0 || nom != 0) phi = atan2(nom, denom) / 2;
	else phi = 0;
	double c = cos(phi);
	double s = sin(phi);
	double c2 = c*c, s2 = s*s, cs = c*s;

	ll0 = c2*a[0][0] + 2 * cs*a[1][0] + s2*a[1][1];
	ll1 = s2*a[0][0] - 2 * cs*a[1][0] + c2*a[1][1];
	if (ll0<ll1) {
		if (phi>0) phi -= M_PI / 2;
		else phi += M_PI / 2;
		l0 = ll1;
		l1 = ll0;
	} else {
		l0 = ll0;
		l1 = ll1;
	}
}

void Region::computeMoments(std::vector<Region> &region, const RegionSet &decomposition) {
	// (2P)
	std::vector<Interval> rle = decomposition.rle;
	Interval I;

	for (unsigned int i = 0; i < rle.size(); ++i){
		I = rle[i];
		if (region.size() == I.region){
			region.push_back(Region());
		}
		region[I.region].integral   += I.xHi - I.xLo + 1;
		region[I.region].integralX  += (I.xHi * (I.xHi + 1) - I.xLo * (I.xLo - 1)) * .5;
		region[I.region].integralY  += (I.xHi - I.xLo + 1) * I.y;
		region[I.region].integralXX += (std::pow(I.xHi + .5, 3) - std::pow(I.xLo - .5, 3)) / 3.0;
		region[I.region].integralXY += (I.xHi * (I.xHi + 1) - I.xLo * (I.xLo - 1)) * I.y * .5;
		region[I.region].integralYY += (I.xHi - I.xLo + 1) * (I.y * I.y + 1.0 / 12.0);
	}
}

void Region::computeFeatures() {
	// (2P)
	double dIntegralXX, dIntegralXY, dIntegralYY, eig1, eig2;

	centerX  = integralX / integral;
	centerY  = integralY / integral;

	dIntegralXX = integralXX / integral - centerX * centerX;
	dIntegralXY = integralXY / integral - centerX * centerY;
	dIntegralYY = integralYY / integral - centerY * centerY;

	eigenDecompositionSymmetric({ { dIntegralXX, 0 }, { dIntegralXY, dIntegralYY } }, mainAxis, eig1, eig2);

	largeLength = 2 * std::sqrt(eig1); // shouldn't that be called small length since it's shorter?
	smallLength = 2 * std::sqrt(eig2); // and this large length?
}


void Region::classify() {
	// (2P)

	// actually should be "largeLength / smallLength"...
	// but for some reason small length is bigger than large length with the formulas given in the lecture...
	// maybe smallLength should be called longLength instead and vice versa?
	
	if (integral >= 5000){
		int ratio = smallLength / largeLength;

		switch (ratio) {
		case 1:
			label = "Plate";
			break;
		case 7:
		case 8:
			label = "Spoon";
			break;
		case 10:
		case 11:
			label = "Fork";
			break;
		case 14:
		case 15:
			label = "Knife";
			break;
		default:
			break;
		}
	}
}