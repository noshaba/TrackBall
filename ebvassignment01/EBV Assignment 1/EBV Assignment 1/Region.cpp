#include "Region.hpp"

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
	//TODO: Implement (2P)
}

void Region::computeFeatures() {
	//TODO: Implement (2P)
}


void Region::classify() {
	//TODO: Implement (2P)
	// Use features computed to label the regions as Teller/Messer/Gabel/Loeffel
	// This function is highly dependent on the concrete setting.
	// It's okay to use fixed thresholds.
}