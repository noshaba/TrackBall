#ifndef _INTERVAL_HPP
#define _INTERVAL_HPP

	//************ Interval, DecompositionIntoRegions

	//! A horizontal stripe (interval) in a binary image.
	class Interval {
		public:
			//! An interval consists of all points (x,y) with xLo<=x && x<=xHi
			int xLo, xHi, y;

			//! index of the connected component of black regions to which this interval belongs
			int region;

			//! auxiliary pointer for union-find. 
			/*! Pointer to an earlier (when scanning rowwise) interval in
			the same region. If it points to itself, this is the first interval.
			*/
			Interval* parent;

			Interval() :xLo(0), xHi(0), y(0), region(0), parent(nullptr) {};
			Interval(int xLo, int xHi, int y) :xLo(xLo), xHi(xHi), y(y), region(0), parent(nullptr) { }
	};

#endif