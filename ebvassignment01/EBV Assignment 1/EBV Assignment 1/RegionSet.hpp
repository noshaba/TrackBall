#ifndef _REGIONSET_HPP
#define _REGIONSET_HPP

#include "Interval.hpp"
#include "Precompiled.hpp"

	//! An black an white image decomposed into horizontal Intervals
	/*! Each interval belongs to a certain region.
	*/
	class RegionSet {
		public:
			std::vector<Interval> rle;

			//! Binarizes and run length codes image. 
			/*! After execution rle contains all horizontal intervals of pixels
			BELOW threshold that are at least minLength long. The intervals are sorted by top to bottom and
			within the same y coordinate left to right. All regions are set to 0.
			*/
			void thresholdAndRLE(cv::Mat_<uchar>& image, uchar threshold, int minLength);

			//! Finds connected regions in the rle intervals 
			/*! Label the regions by setting \c .region in all intervals of a
			connected region. The regions are numbered consecutively.
			*/
			void groupRegions();

		protected:
			//! Auxiliary routine for groupRegions
			/*! Returns, whether run and flw touch and run is one line below follow. */
			bool touch(Interval* run, Interval* flw);

			//! Auxiliary routine for groupRegions
			/*! Returns, whether run is one line below flw and horizontally ahead
			or more than one line below flw. */
			bool ahead(Interval* run, Interval* flw);

			//! Auxiliary routine for unite. 
			/*! Finds the root of \c iv. This is the first interval of the
			connected component. It modifies all intervals along the way to
			point directly to the final parent.
			*/
			void pathCompress(Interval* iv);

			//! Auxiliary routine for group regions. 
			/*! Unites the connected components of iv1 and iv2.
			*/
			void unite(Interval* iv1, Interval* iv2);

			//! Auxiliary routine for groupRegion
			/*! Initializes the .parent pointer of all intervals to point to itself,
			declaring every interval as a single region.
			*/
			void initialize();

			//! Auxiliary routine for groupRegion
			/*! Assumes that the regions are defined by the .parent pointer
			(All interval having the same root are one region) and gives
			the regions (in .region) consecutives numbers starting with 0.
			*/
			void setRegionIndex();
	};

#endif