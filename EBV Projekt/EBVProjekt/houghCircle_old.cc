#include "houghCircle.h"

using namespace std;

HoughCircle::Parameters::Parameters()
 :rMin(10), rMax(15), sobelThreshold(7), houghThreshold(7), radiusHoughThreshold(0), localMaxRange(100)
{
	// so that findBestRadius and other functions do not have a buffer overflow
	if (rMin < 0) rMin = 0;
	if (rMax < 0) rMax = 0;
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
    // implement (2P)
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
	float ang;
	int angle;
	for (int y = -128; y < 128; ++y) for (int x = -128; x < 128; ++x) {
		ang = atan2(y, x);
		if (ang < 0) ang += M_PI;
		angle = (int)round(ang * Parameters::NR_OF_ORIENTATIONS / M_PI);
		if (angle == Parameters::NR_OF_ORIENTATIONS) angle = 0;
		sobelTab.push_back(SobelEntry((int)round(sqrt(sq(x) + sq(y))), angle, cos(ang), sin(ang)));
	}
	// init relative address LUT
	relativeAddressForAngleAndR.clear();
	int dX, dY;
	for (int angle = 0; angle < Parameters::NR_OF_ORIENTATIONS; ++angle) for (int r = 0; r <= param.rMax; ++r){
		ang = M_PI*angle / Parameters::NR_OF_ORIENTATIONS;
		dX = r * cos(ang);
		dY = r * sin(ang);
		relativeAddressForAngleAndR.push_back(dX + houghImgWidthStep * dY);
	}
	// error handling
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
    // implement (1P)
	int relAddr;
	for (int r = param.rMin; r <= param.rMax; ++r) {
		relAddr = relativeAddressForAngleAndR[sobelTab[sobelCoded].angle*(param.rMax+1)+r];
		// along normal
		houghImgOrigin[y * houghImgWidthStep + x + relAddr]++;
		// opposed normal
		houghImgOrigin[y * houghImgWidthStep + x - relAddr]++;
	}
}


void HoughCircle::hough (Mat_<ushort>& houghImg, const Mat_<ushort>& sobelImgPrev, const Mat_<ushort>& sobelImg) const
{
    // implement (1P)
	// error handling
	assert("sobelPrev and sobel must have the same size" && sobelImgPrev.rows == sobelImg.rows && sobelImgPrev.cols == sobelImg.cols);
	int sobelLen, sobelLenPrev;
	// calculate origin of hough image
	ushort* origin = houghImg.ptr<ushort>(param.rMax)+param.rMax;
	// for more performance
	#pragma omp for
	for (int y = 0; y < sobelImg.rows; ++y) {
		for (int x = 0; x < sobelImg.cols; ++x) {
			// calculate sobel length of previous sobelImg
			sobelLenPrev = sobelTab[sobelImgPrev(y, x)].length;
			// calculate sobel length of sobelImg
			sobelLen = sobelTab[sobelImg(y, x)].length;
			// wasn't sure if > or >= since in line 97 in the header "exceed" is written
			// but in line 22 it says it ignore every value below sobelThreshold
			if ((sobelLen - sobelLenPrev) > param.sobelThreshold)
				addPointToAccumulator(origin, x, y, sobelImg(y,x));
		}
	}
}


bool HoughCircle::isLocalMaximum (const Mat_<ushort>& houghImg, int xC, int yC) const
{
    // implement (2P)
	const ushort *hLine = nullptr;
	const ushort center = houghImg.ptr<ushort>(yC)[xC];
	int x, y;
	// [+localMaxRange,-localMaxRange]
	for (int dy = -param.localMaxRange; dy <= param.localMaxRange; ++dy) {
		// check if no edge exceeding
		y = yC + dy;
		if (y >= 0 && y < houghImgHeight){
			hLine = houghImg.ptr<ushort>(y);
			for (int dx = -param.localMaxRange; dx <= param.localMaxRange; ++dx) {
				x = xC + dx;
				// check if no edge exceeding and return false if greater value was found
				if (x >= 0 && x < houghImgWidth)
					if(center < hLine[x] || center == hLine[x] && (y<yC || (y==yC && x<xC)))
						return false;
			}
		}
	}
	return true;
}


int HoughCircle::findBestRadius (const Mat_<ushort>& sobelImg, int xC, int yC, int& bestR) const
{
    // implement (2P)

	// init hough R
	std::vector<int> houghR;
	for (int r = 0; r <= param.rMax; ++r)
		houghR.push_back(0);

	// increase houghR at r if fitting value was found
	SobelEntry s;
	int r; float d;
	for (int dY = -param.rMax; dY <= param.rMax; dY++)
		for (int dX = -param.rMax; dX <= param.rMax; dX++) {
			// check for edge exceeding
			if (xC + dX >= 0 && xC + dX < sobelImg.cols &&
				yC + dY >= 0 && yC + dY < sobelImg.rows) {
				s = sobelTab[sobelImg(yC + dY, xC + dX)];
				// calculate radius
				r = (int)round(fabs(s.cosAngle *dX + s.sinAngle*dY));
				d = fabs(-s.sinAngle*dX + s.cosAngle*dY);
				if (d <= 1 && s.length > param.sobelThreshold && r <= param.rMax)
					houghR[r]++; // increase if good value
			}
		}

	// find best radius and valueR
	bestR = param.rMin;
	int bestVal = 0;
	for (int r = param.rMin; r <= param.rMax; r++) {
		if (houghR[r]>houghR[bestR]) { 
			bestR = r; 
			bestVal = houghR[r];
		}
	}

	return bestVal;
}


void HoughCircle::extractFromHoughImage (vector<Circle>& circles, const Mat_<ushort>& houghImg, const Mat_<ushort>& sobelImg) const
{
    // implement (1P)
	circles.clear();
	const ushort* pLine = nullptr;
	int bestR;
	int valueR;
	// starts at +RMAX and ends at end-RMAX because the hough room is 2*RMAX in both dimensions
	// greater than the original image
	for (int y = param.rMax; y < houghImgHeight - param.rMax; ++y) {
		pLine = houghImg.ptr<ushort>(y);
		for (int x = param.rMax; x < houghImgWidth - param.rMax; ++x) {
			// find a high enough entry as center
			if (pLine[x] >= param.houghThreshold && isLocalMaximum(houghImg, x, y)) {
				valueR = findBestRadius(sobelImg, x - param.rMax, y - param.rMax, bestR);
				// entry has to be at least radiusHoughThreshold (line 25 in header)
				if (valueR >= param.radiusHoughThreshold)
					// calculate hough img coordinated to original img coordinates
					circles.push_back(Circle(x - param.rMax, y - param.rMax, bestR, pLine[x], valueR));
			}
		}
	}
}


void HoughCircle::findCircles (vector<Circle> &circles, Mat_<ushort>& houghImg, const Mat_<ushort>& sobelPrev, const Mat_<ushort>& sobel) const
{
    // implement (1P)
	// error handling
	assert("sobel images must be the same size" && sobel.cols == sobelPrev.cols && sobel.rows == sobelPrev.rows);
	circles.clear();
	// create hough image if not allocated
	if (houghImg.empty())
		houghImg = createHoughImage();
	#pragma omp parallel
	{
		// apply hough transform
		hough(houghImg, sobelPrev, sobel);
	}
	// create circle out of transform
	extractFromHoughImage(circles, houghImg, sobel);
}

