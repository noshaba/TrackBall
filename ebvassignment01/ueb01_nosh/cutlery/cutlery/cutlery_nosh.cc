#ifdef _WIN32
#define WIN32
#endif
#ifdef WIN32
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>

#include <assert.h>
#include <vector>


using namespace std;
using namespace cv;
typedef double Matrix2x2[2][2];


//************ Interval, DecompositionIntoRegions

//! A horizontal stripe (interval) in a binary image.
class Interval
{
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

	Interval() :xLo(0), xHi(0), y(0), region(0), parent(NULL) {};
	Interval(int xLo, int xHi, int y) :xLo(xLo), xHi(xHi), y(y), region(0), parent(NULL) { }
};


//! An black an white image decomposed into horizontal Intervals
/*! Each interval belongs to a certain region.
*/
class RegionSet
{
public:
	vector<Interval> rle;

	//! Binarizes and run length codes image. 
	/*! After execution rle contains all horizontal intervals of pixels
	BELOW threshold that are at least minLength long. The intervals are sorted by top to bottom and
	within the same y coordinate left to right. All regions are set to 0.
	*/
	void thresholdAndRLE(Mat_<uchar>& image, uchar threshold, int minLength);

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



void RegionSet::thresholdAndRLE(Mat_<uchar>& image, uchar threshold, int minLength)
{
	unsigned int length;
	for (unsigned int i = 0; i < image.rows; ++i){
		length = 0;
		for (unsigned int j = 0; j < image.cols; ++j){
			if (image(i, j) > threshold){
				if (length >= minLength)
					rle.push_back(Interval(j - length, j - 1, i));
				image(i, j) = 255;
				length = 0;
			}
			else {
				image(i, j) = 0;
				++length;
			}
		}
		if (length >= minLength)
			rle.push_back(Interval(image.cols - length, image.cols - 1, i));
	}
}


void RegionSet::pathCompress(Interval* iv)
{
	Interval *root, *buffer;
	root = iv;
	while (root->parent != root) root = root->parent;
	while (iv != root) {
		buffer = iv->parent;
		iv->parent = root;
		iv = buffer;
	}
}

void RegionSet::unite(Interval* iv1, Interval* iv2)
{
	pathCompress(iv1); pathCompress(iv2);
	iv1->parent < iv2->parent ? iv2->parent->parent = iv1->parent : iv1->parent->parent = iv2->parent;
}


void RegionSet::initialize()
{
	std::vector<Interval>::iterator it;
	for (it = rle.begin(); it != rle.end(); it++) it->parent = &(*it);
}


void RegionSet::setRegionIndex()
{
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

void RegionSet::groupRegions()
{
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


//************* a region and it's features

//! All features extracted for a region
class Region
{
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
	string label;

	//!constructor for an empty region
	Region() :integral(0), integralX(0), integralY(0), integralXX(0), integralXY(0), integralYY(0),
		centerX(0), centerY(0), mainAxis(0), smallLength(0), largeLength(0)
	{}

	//! Compute second order moments from run length code for each region
	/*! Sets \c Integral, \c IntegralX, \c IntegralY, \c IntegralXX, \c
	IntegralXY, \c IntegralYY in \c region from \c decomposition
	*/
	static void computeMoments(vector<Region>& region, const RegionSet& decomposition);

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


void Region::eigenDecompositionSymmetric(const Matrix2x2& a, double& phi, double& l0, double& l1)
{
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
	}
	else {
		l0 = ll0;
		l1 = ll1;
	}
}

void Region::computeMoments(vector<Region> &region, const RegionSet &decomposition)
{
	std::vector<Interval> rle = decomposition.rle;
	Interval I;

	for (unsigned int i = 0; i < rle.size(); ++i){
		I = rle[i];
		if (region.size() == I.region){
			region.push_back(Region());
		}
		region[I.region].integral += I.xHi - I.xLo + 1;
		region[I.region].integralX += (I.xHi * (I.xHi + 1) - I.xLo * (I.xLo - 1)) * .5;
		region[I.region].integralY += (I.xHi - I.xLo + 1) * I.y;
		region[I.region].integralXX += (std::pow(I.xHi + .5, 3) - std::pow(I.xLo - .5, 3)) / 3.0;
		region[I.region].integralXY += (I.xHi * (I.xHi + 1) - I.xLo * (I.xLo - 1)) * I.y * .5;
		region[I.region].integralYY += (I.xHi - I.xLo + 1) * (I.y * I.y + 1.0 / 12.0);
	}
}

void Region::computeFeatures()
{

	double dIntegralXX, dIntegralXY, dIntegralYY, eig1, eig2;

	centerX = integralX / integral;
	centerY = integralY / integral;

	dIntegralXX = integralXX / integral - centerX * centerX;
	dIntegralXY = integralXY / integral - centerX * centerY;
	dIntegralYY = integralYY / integral - centerY * centerY;

	eigenDecompositionSymmetric({ { dIntegralXX, dIntegralXY }, { dIntegralXY, dIntegralYY } }, mainAxis, eig1, eig2);

	largeLength = 2 * std::sqrt(eig1);
	smallLength = 2 * std::sqrt(eig2);
}



void Region::classify()
{
	if (integral >= 5000){
		int ratio = largeLength / smallLength;

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




//************ GUI



//! paint the extracted Intervals in \c rle onto \c img for displaying
/*! The color is chosen with a cyclic color table according to
\c .region
*/
void paintDecomposition(Mat& img, const RegionSet& decomposition)
{
	Scalar color[6] =
	{
		CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(255, 255, 0), CV_RGB(0, 0, 255), CV_RGB(255, 0, 255), CV_RGB(0, 255, 255)
	};
	for (int i = 0; i<(int)decomposition.rle.size(); i++) {
		line(img, Point(decomposition.rle[i].xLo, decomposition.rle[i].y),
			Point(decomposition.rle[i].xHi, decomposition.rle[i].y),
			color[decomposition.rle[i].region % 6]);
	}
}


//! Paint the center of gravity / intertial axis and labels for all regions onto \c img
/*! The routine paints a cross at the center with the lines of the cross corresponding
to the radii of an ellipse that would have the asme intertial properties.
*/
void paintFeatures(Mat& img, const vector<Region>& features)
{
	for (int i = 0; i<(int)features.size(); i++) {
		const Region* rg = &features[i];
		double c = cos(rg->mainAxis), s = sin(rg->mainAxis);
		// paint +/-largeLenge in direction (c,s) from the center of gravity
		line(img,
			Point((int)(rg->centerX + rg->largeLength*c), (int)(rg->centerY + rg->largeLength*s)),
			Point((int)(rg->centerX - rg->largeLength*c), (int)(rg->centerY - rg->largeLength*s)),
			CV_RGB(255, 255, 255));
		// paint +/-smallLength in direction (-s,c) orthogonal to (c,s) from the center of gravity
		line(img,
			Point((int)(rg->centerX - rg->smallLength*s), (int)(rg->centerY + rg->smallLength*c)),
			Point((int)(rg->centerX + rg->smallLength*s), (int)(rg->centerY - rg->smallLength*c)),
			CV_RGB(255, 255, 255));
		putText(img, rg->label, Point((int)rg->centerX, (int)rg->centerY),
			FONT_HERSHEY_PLAIN, 2, CV_RGB(255, 255, 255));
	}
}


int main(int argc, char** argv)
{
	namedWindow("Segmented Image", CV_WINDOW_NORMAL);
	for (int i = 0; 1 + i<argc; i++) {
		const char* filename = argv[i + 1];
		// Load the source image as a grayscale image for processing
		Mat_<uchar> src = imread(filename, 0); // 0 means load as grayscale
		// Load the same image as a color image for displaying and painting onto
		Mat dst = imread(filename, 1); // 1 means load as color (for displaying)

		if (!src.data)
		{
			printf("Image could not be loaded.\n");
			return -1;
		}

		// measure time
		int64 time1, time2;

		// Do the computer vision computation
		time1 = getTickCount();
		RegionSet decomposition;
		int threshold = 80;
		int minLength = 2;
		decomposition.thresholdAndRLE(src, threshold, minLength);
		decomposition.groupRegions();
		vector<Region> region;
		Region::computeMoments(region, decomposition);
		for (int i = 0; i<(int)region.size(); i++) {
			region[i].computeFeatures();
			region[i].classify();
		}
		time2 = getTickCount();


		// measure time
		printf("Computation time %fs\n", (time2 - time1) / getTickFrequency());

		// paint everything
		paintDecomposition(dst, decomposition);
		paintFeatures(dst, region);

		// show as a window in the GUI
		imshow("Segmented Image", dst);
		waitKey(0);
		//imwrite ("result.png", dst);   // save the segmented and labeled result    
	}
	return 0;
}
