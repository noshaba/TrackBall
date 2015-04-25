#include "RegionSet.hpp"

void RegionSet::thresholdAndRLE(cv::Mat_<uchar>& image, uchar threshold, int minLength) {
	//TODO: Implement (2P)
}


void RegionSet::pathCompress(Interval* iv) {
	// TODO: Implement
}

void RegionSet::unite(Interval* iv1, Interval* iv2) {
	// TODO: Implement
}


void RegionSet::initialize() {
	//TODO: Implement 
}


void RegionSet::setRegionIndex() {
	// TODO: Implement
}


bool RegionSet::touch(Interval* run, Interval* flw) {
	// TODO: Implement
	return false;
}

bool RegionSet::ahead(Interval* run, Interval* flw) {
	// TODO: Implement
	return false;
}

void RegionSet::groupRegions() {
	// TODO: Implement (3p with functions from pathCompress to setRegionIndex)
}