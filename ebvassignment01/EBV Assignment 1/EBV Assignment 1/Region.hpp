#ifndef _REGION_HPP
#define _REGION_HPP

#include "RegionSet.hpp"

	//************* a region and it's features

	//! All features extracted for a region
	class Region {
		public:
			//! Sum of 1 (area), x, y, x^2, xy and y^2
			/*! The sum is taken over all pixel in the region.  \c integral can be
			directly used as area. \c integralX and \c integralY are used to compute the
			center of gravity (\c centerX, \c centerY) \c integralXX, \c integralXY and \c
			integralYY are used to compute the \c mainAxis and corresponding \c
			smallLength and \c largeLength.
			*/
			double integral, integralX, integralY, integralXX, integralXY, integralYY;

			//! center of gravity of the region
			double centerX, centerY;

			//! Angle of the larges inertial extension.
			/*! \c smallLength and \c largeLength are the radii of an ellipse
			that would have the same inertial properties.
			*/
			double mainAxis, smallLength, largeLength;

			//! Label (Teller, Messer, Gabel, Loeffel) assigned to the region
			std::string label;

			//!constructor for an empty region
			Region() :integral(0), integralX(0), integralY(0), integralXX(0), integralXY(0), integralYY(0),
				centerX(0), centerY(0), mainAxis(0), smallLength(0), largeLength(0)
			{}

			//! Compute second order moments from run length code for each region
			/*! Sets \c Integral, \c IntegralX, \c IntegralY, \c IntegralXX, \c
			IntegralXY, \c IntegralYY in \c region from \c decomposition
			*/
			static void computeMoments(std::vector<Region>& region, const RegionSet& decomposition);

			//! Compute center and inertial axes from the second order moments
			/*! Sets \c centerX, \c centerY, \c mainAxis, \c smallLength, \c
			largeLength. */
			void computeFeatures();

			//! Determine label from area and inertial axes
			/*! Sets \c label */
			void classify();

		protected:
			//! Auxiliary routine for finding inertial axis.
			/*! Computes an eigenvalue decomposition of a symmetric 2x2 matrix
			in a way, that is stable even for degenerated eigenvalues
			\c (cos(phi), sin(phi)) will be an eigenvector with eigenvalue
			\c l0 and \c (-sin(phi),cos(phi)) will be the orthogonal
			eigenvector with eigenvalue \c l1. \c l0>=l1.
			*/
			static void eigenDecompositionSymmetric(const Matrix2x2& a, double& phi, double& l0, double& l1);
	};

#endif