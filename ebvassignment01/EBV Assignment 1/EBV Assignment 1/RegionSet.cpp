#include "RegionSet.hpp"

void RegionSet::thresholdAndRLE(cv::Mat_<uchar>& image, uchar threshold, int minLength) {
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
	Interval *root, *buffer;
	root = iv;
	while (root->parent != root) root = root->parent;
	while (iv != root) {
		buffer = iv->parent;
		iv->parent = root;
		iv = buffer;
	}
}

void RegionSet::unite(Interval* iv1, Interval* iv2) {
	pathCompress (iv1); pathCompress (iv2);
	iv1->parent < iv2->parent ? iv2->parent->parent = iv1->parent : iv1->parent->parent = iv2->parent;
}


void RegionSet::initialize() {
	std::vector<Interval>::iterator it;
	for (it = rle.begin(); it != rle.end(); it++) it->parent = &(*it);
}


void RegionSet::setRegionIndex() {
	std::vector<Interval>::iterator iv;
	iv = rle.begin();
	int regionCtr = 0;
	while (iv != rle.end()) {
		if (iv->parent == &(*iv)) {
			iv->region = regionCtr;
			regionCtr++;
		}
		else iv->region = iv->parent->region;
		iv++;
	}
}


bool RegionSet::touch(Interval* run, Interval* flw) {
	return run->y == flw->y + 1 && run->xHi >= flw->xLo && flw->xHi >= run->xLo;
}

bool RegionSet::ahead(Interval* run, Interval* flw) {
	return (run->y > flw->y + 1) || (run->y == flw->y + 1 && run->xHi > flw->xHi);
}

void RegionSet::groupRegions() {
	initialize();
	std::vector<Interval>::iterator flw, run;
	flw = run = rle.begin();
	while (run != rle.end()) {
		if (touch(&(*run), &(*flw))) unite(&(*run), &(*flw));
		if (ahead(&(*run), &(*flw))) flw++;
		else run++;
	}
	setRegionIndex();
}