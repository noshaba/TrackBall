#include "houghCircle.h"

using namespace std;

HoughCircle::Parameters::Parameters()
// TODO: choose good parameters
//SAMPLE :rMin(0), rMax(0), sobelThreshold(0), houghThreshold(0), radiusHoughThreshold(0), localMaxRange(0)

//SOLUTION
:rMin(7), rMax(14), sobelThreshold(7), houghThreshold(20), radiusHoughThreshold(10), localMaxRange(12)
//ENDSOLUTION
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
    //SOLUTION
    imgWidth    = width;
    imgHeight   = height;
    this->param = param;
    
    int nO = Parameters::NR_OF_ORIENTATIONS;
    
    // Build sobelTab
    sobelTab.reserve (0x10000); // 0x10000 different values for a 16bit coded sobel vector
    for (int sobelCoded=0; sobelCoded<0x10000; sobelCoded++) {
        int sobelX = sobelXUncode (sobelCoded);
        int sobelY = sobelYUncode (sobelCoded);
        // store entry corresponding to 'sobelX, sobelY'
        double sobelLength = sqrt ((double) (sobelX*sobelX + sobelY*sobelY));
        double sobelAngle  = atan2 ((double) sobelY, (double) sobelX);
        if (sobelAngle<0) sobelAngle += M_PI; // normalize to M_PI
        short angleIdx = (short) round(nO*sobelAngle/M_PI);
        if (angleIdx==nO) angleIdx-=nO;
        assert (sobelXUncode(sobelTab.size())==sobelX);
        assert (sobelYUncode(sobelTab.size())==sobelY);
        
        sobelTab.push_back (SobelEntry ((int) round(sobelLength), angleIdx,
                                        cos(sobelAngle), sin(sobelAngle)));
    }
    
    // build 'relativeAddressForAngleAndR'
    houghImgWidth     = imgWidth + 2*param.rMax;
    houghImgHeight    = imgHeight + 2*param.rMax;
    Mat_<ushort> dummyImage = createHoughImage ();
    houghImgWidthStep = dummyImage.step[0]/dummyImage.step[1];
    relativeAddressForAngleAndR.reserve ((param.rMax+1)*nO);
    for (int angleIdx=0; angleIdx<nO; angleIdx++)
        for (int r=0; r<=param.rMax; r++) {
            double angle = M_PI*angleIdx/nO;
            int dx = (int) floor(0.5+r*cos(angle));
            int dy = (int) floor(0.5+r*sin(angle));
            relativeAddressForAngleAndR.push_back(dx+dy*houghImgWidthStep);
        }
    //ENDSOLUTION
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
    // FAST CODE optimized to the greatest extend sensible
    //SOLUTION
    const SobelEntry& se = sobelTab[sobelCoded];
    ushort* houghBase = houghImgOrigin + x + y*houghImgWidthStep;
    const int* ra = &relativeAddressForAngleAndR[se.angle*(param.rMax+1)];
    const int* pRa = &ra[param.rMin];
    const int* pEndRa = &ra[param.rMax+1];
    while (pRa!=pEndRa) {
        int rra = *pRa;
        houghBase[rra] ++;
        houghBase[-rra]++;
        pRa++;
    }
    //ENDSOLUTION
}


void HoughCircle::hough (Mat_<ushort>& houghImg, const Mat_<ushort>& sobelImgPrev, const Mat_<ushort>& sobelImg) const
{
    // TODO: implement (1P)
    // FAST CODE optimized to the greatest extend sensible
    //SOLUTION
    assert ("sobelImg has incorrect width"
            && imgWidth  == sobelImg.cols);
    assert ("sobelImg has incorrect height"
            && imgHeight == sobelImg.rows);
    assert ("sobelPrev has incorrect width"
            && imgWidth  == sobelImgPrev.cols);
    assert ("sobelPrev has incorrect height"
            && imgHeight == sobelImgPrev.rows);
    int w = sobelImg.cols;
    ushort* houghImgOrigin = (ushort*) &houghImg(param.rMax, param.rMax);
    for (int y=0; y<sobelImg.rows; y++) {
        const ushort* lineStart = sobelImg[y];
        const ushort* p = lineStart;
        const ushort* pEnd = p + w;
        const ushort* pPrev = sobelImgPrev[y];
        while (p!=pEnd) {
            const SobelEntry& se = sobelTab [*p];
            const SobelEntry& sePrev = sobelTab [*pPrev];
            if (se.length >= sePrev.length + param.sobelThreshold)
                addPointToAccumulator (houghImgOrigin, p-lineStart, y, *p);
            p++;
            pPrev++;
        }
    }
    //ENDSOLUTION
}


bool HoughCircle::isLocalMaximum (const Mat_<ushort>& houghImg, int xC, int yC) const
{
    // TODO: implement (2P)
    // Note that a houghImg pixel is \c (ushort)
    // Pixel \c (x,y) of houghImage can be accessed as \c (ushort*) cvPtr2D (houghImg, yC, xC)
    //SOLUTION
    const ushort* p = &houghImg (yC, xC);
    const ushort compareWith = *p;
    int xLo = xC-param.localMaxRange;
    if (xLo<0) xLo = 0;
    int xHi = xC+param.localMaxRange;
    if (xHi>=houghImg.cols) xHi = houghImg.cols-1;
    int yLo = yC-param.localMaxRange;
    if (yLo<0) yLo = 0;
    int yHi = yC+param.localMaxRange;
    if (yHi>=houghImg.rows) yHi = houghImg.rows-1;
    for (int y2 = yLo; y2<=yHi; y2++)
        for (int x2 = xLo; x2<=xHi; x2++) {
            const ushort* p = &houghImg (y2, x2);
            if (*p>compareWith ||
                (*p==compareWith && (y2<yC || (y2==yC && x2<xC)))) return false;
        }
    return true;
    //ENDSOLUTION
    //SAMPLE return false;
}


int HoughCircle::findBestRadius (const Mat_<ushort>& sobelImg, int xC, int yC, int& bestR) const
{
    // TODO: implement (2P)
    //SOLUTION
    int xLo = xC-param.rMax;
    if (xLo<1) xLo = 1;
    int xHi = xC+param.rMax;
    if (xHi>sobelImg.cols-2) xHi = sobelImg.cols-2;
    int yLo = yC-param.rMax;
    if (yLo<1) yLo = 1;
    int yHi = yC+param.rMax;
    if (yHi>sobelImg.rows-2) yHi = sobelImg.rows-2;
    
    vector<int> hough (2*param.rMax, 0);
    for (int y=yLo; y<=yHi; y++)  {
        const ushort* p = sobelImg[y];
        for (int x=xLo; x<=xHi; x++) {
            const SobelEntry& se = sobelTab [p[x]];
            if (se.length>=param.sobelThreshold) {
                float distR = fabs((x-xC)*se.cosAngle + (y-yC)*se.sinAngle);
                float distT = -fabs((x-xC)*se.sinAngle + (y-yC)*se.cosAngle);
                if (distT<=1) hough[(int) round(distR)]++;
            }
        }
    }
    
    int bestVal = 0;
    bestR = 0;
    for (int r=param.rMin; r<=param.rMax; r++)
        if (hough[r]>bestVal) {
            bestVal = hough[r];
            bestR = r;
        }
    return bestVal;
    //ENDSOLUTION
    //SAMPLE return 0;
}


void HoughCircle::extractFromHoughImage (vector<Circle>& circles, const Mat_<ushort>& houghImg, const Mat_<ushort>& sobelImg) const
{
    // TODO: implement (1P)
    //SOLUTION
    circles.clear();
    for (int y=0; y<houghImg.rows; y++) {
        const ushort* p = houghImg[y];
        const ushort* pEnd = p + houghImg.cols;
        const ushort* lineStart = p;
        while (p!=pEnd) {
            if (*p>=param.houghThreshold && isLocalMaximum (houghImg, p-lineStart, y)) {
                int xC = p-lineStart - param.rMax, yC = y-param.rMax, r;
                int valR = findBestRadius (sobelImg, xC, yC, r);
                if (valR>=param.radiusHoughThreshold) circles.push_back (Circle(xC, yC, r, *p, valR));
            }
            p++;
        }
    }
    //ENDSOLUTION
}


void HoughCircle::findCircles (vector<Circle> &circles, Mat_<ushort>& houghImg, const Mat_<ushort>& sobelPrev, const Mat_<ushort>& sobel) const
{
    // TODO: implement (1P)
    // Note that a houghImg pixel is \c (ushort)
    //SOLUTION
    if (houghImg.empty()) houghImg = createHoughImage();
    hough (houghImg, sobelPrev, sobel);
    extractFromHoughImage (circles, houghImg, sobel);
    //ENDSOLUTION
}

