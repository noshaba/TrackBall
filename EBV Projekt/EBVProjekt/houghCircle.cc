#include "houghCircle.h"

using namespace std;

HoughCircle::Parameters::Parameters()
// TODO: choose good parameters
 :rMin(0), rMax(50), sobelThreshold(80), houghThreshold(10), radiusHoughThreshold(0), localMaxRange(0)

{
}

HoughCircle::HoughCircle ()
:imgWidth(0), imgHeight(0), houghImgWidth(0), houghImgHeight(0), houghImgWidthStep(0)
{}

void HoughCircle::assertSobelTab () const
{
    if (sobelTab.empty()) return;
    assert (sobelTab.size()>=0x10000);
    for (int i=0; i<(int) sobelTab.size(); i++) {
        const SobelEntry&  se = sobelTab[i];
        assert (0<=se.length && se.length<=sqrt(2.0)*128.0+0.01);
        assert (0<=se.angle && se.angle<Parameters::NR_OF_ORIENTATIONS);
        assert (-1<=se.cosAngle && se.cosAngle<=1);
        assert (-1<=se.sinAngle && se.sinAngle<=1);
        assert (fabs(sqrt(sq(se.cosAngle)+sq(se.sinAngle))-1)<1E-5); // norm of (cos,sin) is 1
    }
}


void HoughCircle::assertRelativeAddressForAngleAndRTab () const
{
    if (relativeAddressForAngleAndR.empty()) return;
    assert ((int) relativeAddressForAngleAndR.size()==Parameters::NR_OF_ORIENTATIONS*(param.rMax+1));
    for (int i=0; i<(int) relativeAddressForAngleAndR.size(); i++) {
        assert (abs(relativeAddressForAngleAndR[i])/houghImgWidthStep<param.rMax+1);
    }
}


void HoughCircle::create (int width, int height, const Parameters &param)
{
    // TODO: implement (2P)
    // Hint: i = (int) round(x), round x to the nearest integer i
	imgWidth = width;
	imgHeight = height;
	// + 2 * RMAX to not to exceed edges of image
	houghImgWidth = width + (param.rMax << 1);
	houghImgHeight = height + (param.rMax << 1);
	// create dummy image to determine step size
	Mat_<ushort> dummyImg(houghImgHeight, houghImgWidth);
	houghImgWidthStep = dummyImg.step[0] / dummyImg.step[1];
	//deallocate memory
	dummyImg.release();
	// init sobel angle LUT
	sobelTab.clear();
	float angle;
	for (int x = -128; x < 128; ++x) for (int y = -128; y < 128; ++y) {
		angle = fabs(atan2(y, x))*255.0f / M_PI; // normalize and discretize angle
		sobelTab.push_back(SobelEntry((int)round(sqrt(sq(x) + sq(y))), (int)round(angle), cos(angle), sin(angle)));
	}
	// init relative address LUT
	relativeAddressForAngleAndR.clear();
	int dX, dY;
	for (int angle = 0; angle < Parameters::NR_OF_ORIENTATIONS; ++angle) for (int r = 0; r <= param.rMax; ++r){
		dX = r * cos(angle);
		dY = r * sin(angle);
		relativeAddressForAngleAndR.push_back(dX + houghImgWidthStep * dY);
	}
	assertSobelTab();
    assertRelativeAddressForAngleAndRTab();
}


Mat_<ushort> HoughCircle::createHoughImage () const
{
    if (houghImgWidth==0) return Mat_<ushort>();
    
    Mat_<ushort> houghImg (houghImgHeight, houghImgWidth);
    houghImg.setTo (Scalar(0));
    assert ("houghImg->widthStep must be equal to the value used in the table"
            && (houghImgWidthStep==0 || houghImg.step[0] == (int) (houghImgWidthStep*sizeof(ushort))));
    return houghImg;
}


void HoughCircle::addPointToAccumulator (ushort* houghImgOrigin, int x, int y, int sobelCoded) const
{
    // TODO: implement (1P)
	int relAddr;
	// TODO: Randüberschreitung
	for (int r = param.rMin; r <= param.rMax; ++r) {
		relAddr = relativeAddressForAngleAndR[sobelTab[sobelCoded].angle*(param.rMax+1)+r];
		// along normal
		houghImgOrigin[ relAddr] += 1;
		// opposed normal
		houghImgOrigin[-relAddr] += 1;
	}
}


void HoughCircle::hough (Mat_<ushort>& houghImg, const Mat_<ushort>& sobelImgPrev, const Mat_<ushort>& sobelImg) const
{
    // TODO: implement (1P)
	assert("sobelPrev and sobel must have the same size" && sobelImgPrev.rows == sobelImg.rows && sobelImgPrev.cols == sobelImg.cols);
	const ushort* pLine = nullptr;
	int sobelX, sobelY, sobelLen, sobelLenPrev;

	for (int y = 0; y < sobelImg.rows; ++y){
		for (int x = 0; x < sobelImg.cols; ++x){
			pLine = sobelImg.ptr<ushort>(y);
			// TODO: Make that shit faster
			// calculate sobel length of previous sobelImg
			sobelVector(sobelImgPrev, x, y, sobelX, sobelY);
			sobelLenPrev = sqrt(sq(sobelX) + sq(sobelY));
			// calculate sobel length of sobelImg
			sobelVector(sobelImg, x, y, sobelX, sobelY);
			sobelLen = sqrt(sq(sobelX) + sq(sobelY));
			// TODO: > or >= ?!
			if (sobelLen - sobelLenPrev > param.sobelThreshold)
				addPointToAccumulator(&(houghImg.ptr<ushort>(param.rMax)[param.rMax]),x,y,sobelCode(sobelX,sobelY));
		}
	}
}


bool HoughCircle::isLocalMaximum (const Mat_<ushort>& houghImg, int xC, int yC) const
{
    // TODO: implement (2P)
    // Note that a houghImg pixel is \c (ushort)
    // Pixel \c (x,y) of houghImage can be accessed as \c (ushort*) cvPtr2D (houghImg, yC, xC)
 return false;
}


int HoughCircle::findBestRadius (const Mat_<ushort>& sobelImg, int xC, int yC, int& bestR) const
{
    // TODO: implement (2P)
 return 0;
}


void HoughCircle::extractFromHoughImage (vector<Circle>& circles, const Mat_<ushort>& houghImg, const Mat_<ushort>& sobelImg) const
{
    // TODO: implement (1P)
}


void HoughCircle::findCircles (vector<Circle> &circles, Mat_<ushort>& houghImg, const Mat_<ushort>& sobelPrev, const Mat_<ushort>& sobel) const
{
    // TODO: implement (1P)
    // Note that a houghImg pixel is \c (ushort)

	circles.clear();
	if (houghImg.empty())
		houghImg = createHoughImage();

	hough(houghImg, sobelPrev, sobel);

}

