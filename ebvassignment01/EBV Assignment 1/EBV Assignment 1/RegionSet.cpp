#include "RegionSet.hpp"

void RegionSet::thresholdAndRLE(cv::Mat_<uchar>& image, uchar threshold, int minLength) {
	//TODO: Implement (2P)
	unsigned int length;
	for (unsigned int i = 0; i < image.rows; ++i){
		length = 0;
		for (unsigned int j = 0; j < image.cols; ++j){
			if (image(i, j) > threshold){
				if (length >= minLength)
					rle.push_back(Interval(j - length, j - 1, i));
				image(i, j) = 255;
				length = 0;
			} else {
				image(i, j) = 0;
				++length;
			}
		}
		if (length >= minLength)
			rle.push_back(Interval(image.cols - length, image.cols - 1, i));
	}
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