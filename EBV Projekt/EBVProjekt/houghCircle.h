#ifndef HOUGH_CIRCLE
#define HOUGH_CIRCLE

#include "central.h"
#include <vector>

#include "sobel.h"


//! Contains the algorithms and lookup-tables for circle hough transform
/*! Each object corresponds to a fixed \c width*height image format and
    fixed parameter for the circular hough transform algorithm.
 */
class HoughCircle 
{
 public:
  //! Empty object
  HoughCircle ();

  //! Parameter for the algorithms (threshold, min max ranges, etc.)
  /*! The algorithm looks for circles with radius between \c
      rMin..rMax. It ignores pixels with a sobel length below \c
      sobelThreshold and accepts something as a circle if the Hough
      response is at least \c houghThreshold in the XY Hough image and
      at least \c radiusHoughThreshold in the following r-Hough
      transform to determine the radius. We accept only hough entries
      that are locally maximal within +/- \c localMaxRange.
  */
  class Parameters 
  {
  public:
    //! Smallest an largest radius to be searched for
    int rMin, rMax;

    //! Accept a pixel as an edge if the sobel length is above or equal
    int sobelThreshold;

    //! Accept a circle if it's hough entry is above or equal...
    int houghThreshold;

    //! ..and the hough entry in the radius houghspace is above or equal
    int radiusHoughThreshold;    

    //! ..and it's a local maximum within a range of 
    int localMaxRange;    

    //! Angles are discretized to mapping [0..M_PI)-->[0..NR_OF_ORIENTATIONS)
    enum {NR_OF_ORIENTATIONS=256};

    Parameters();    
  };  

  //! The parameters used by the algorithm
  Parameters param;  
  
  //! source image width and height
  int imgWidth, imgHeight;
  
  //! Create the look-up tables for an \c width*height image
  /*! 
      The only clean way to set \c houghImgWidthStep is to allocate (and release) a
      dummy image and compute \c houghImgWidthStep from \c dummyImage->widthStep.
   */
  void create (int width, int height, const Parameters& param=Parameters());

  //! A Circle detected in the image
  class Circle 
  {
  public:
    //! center in pixel
    int xC, yC;

    //! radius in pixel
    int r;

    //! value in the XY Hough accumulator (just for information)
    int value;

    //! value in the R Hough accumulator (just for information)
    int valueR;    

    //! Std. Constructor
    Circle (int xC, int yC, int r, int value, int valueR)
      :xC(xC), yC(yC), r(r), value(value), valueR(valueR)
      {}  
  };

  //! Perform circular Hough-transform on an image where \c sobel is the sobel response.
  /*! consider only pixel where the sobel length is by \c param.sobelThreshold larger than in the
    previous image \c sobelPrev. If \c houghImg is empty, the hough image is allocated and the pointer stored in \c houghImg.
  */
  void findCircles (vector<Circle>& circles, Mat_<ushort>& houghImg, const Mat_<ushort>& sobelPrev, const Mat_<ushort>& sobel) const;

  //! Allocates a hough image of appropriate size.
  Mat_<ushort> createHoughImage () const;
  
  //! Go through the sobel gradient image 'sobelImg' and add any pixel exceeding a sobel
  //! length of sobelThreshold to the Hough accumulator \c houghImg using \c addPointToHoughAccumulator.
  /*! The look up tables are employed to improve performance. 

      Utilize the fact, that the ball is moving by incorporating the
      previous Sobel image \c sobelImgPrev into the decision, whether
      to add a point to the hough accumulator.
   
      FAST CODE optimized to the greatest
      extend sensible.
  */
  void hough (Mat_<ushort>& houghImg, const Mat_<ushort>& sobelImgPrev, const Mat_<ushort>& sobelImg) const;

   //! Find all circles in the Hough image.
   /*! Finds circle in the hough image by running through the image and adding a circle for every
       hough entry that is >=houghThreshold and maximal in a range of +/- \c localMaxRange
       For each circle added the best radius is determined using \c findBestRadius. For testing the
       implementation without having \c findBestRadius implemented, the radius can be set to 0, in
       which case a cross is drawn by the GUI.
       FAST CODE optimized to the greatest extend sensible
   */
  void extractFromHoughImage (vector<Circle>& circles, const Mat_<ushort>& houghImg, const Mat_<ushort>& sobelImg) const;


 protected:

  //! Width and height of the houghCircle image
  int houghImgWidth, houghImgHeight;  

  //! Length of a line in the houghCircle image in \c ushort
  /*! This is the the smallest even number >= \c houghImgWidth.
      Note that the corresponding IplImage->widthStep entry is twice
      as large since it is defined in bytes.
  */
  int houghImgWidthStep;  


  //! An entry of the Sobel table.
  /*! It contains length and angle for a given sobelX, sobelY
      response. 
  */
  class SobelEntry 
  {
  public:
    //! Euclidean norm, i.e. length of the \c (sobelX, sobelY) vector (rounded)
    short length;

    //! angle of \c (sobelX, sobelY) normalized to [0..M_PI)
    //! and discretized to [0..NR_OF_ORIENTATIONS)    
    short angle;

    //! cos and sin of the angle (represented by \c angle)
    //! in usual scale [-1..+1]
    float cosAngle, sinAngle;
    
    SobelEntry ()
      :length(0), angle(0), cosAngle(0), sinAngle(0)
    {}    
      SobelEntry (short length, short angle, float cosAngle, float sinAngle)
        :length(length), angle(angle), cosAngle(cosAngle), sinAngle(sinAngle)
    {}    
  };
  
  //! Look up table of a sobel entry for each sobelX and sobelY combination.
  /*! The index to this table is directly the code 16bit value in the Sobel image.
  */
  vector<SobelEntry> sobelTab;
  

  //! Relative address of a given angle and radius in the innermost Hough loop
  /*! \c relativeAddressForAngleAndR[angleIdx*(RMAX+1) + r] is the
      relative address in the hough accumulator image of moving
      \c (r*cos(angle), r*sin(angle)).
  */
  vector<int> relativeAddressForAngleAndR;

  //! Asserts a sensible range of \c sobelTab values
  void assertSobelTab () const;
  
  //! Asserts a sensible range of \c relativeAddressForAngleAndR values
  void assertRelativeAddressForAngleAndRTab () const;  

  //! Increase the hough accumulator for a fixed pixel \c x,y
  /*! At \c 'x,y' a significant edge with sobel response \c sobelCoded has been found
      now increase the affected accumulator pixels.
      The lookup tables are used to improve performance. \c houghImgOrigin is a
      pointer to the Hough image pixel corresponding to image pixel 0,0. (Note, that
      the Hough image encompasses circle centers up to (-rMax,-rMax).
      FAST CODE optimized to the greatest extend sensible.
  */
  void addPointToAccumulator (ushort* houghImgOrigin, int x, int y, int sobelCoded) const;  


  //! Check, whether (xC,yC) (Hough accumulator coordinates) is maximal in
  //! a range of +/- \c localMaxRange. 
  /*! This routine isn't called very often, so it need not be optimized so much. 
      When there are several identical hough values the routine must consistently
      choose one of them.    
   */
  bool isLocalMaximum (const Mat_<ushort>& houghImg, int xC, int yC) const;

   //! For a given circle center (xC, yC) (image coordinates) find the best radius 'bestR' by
   //! performing a 1D hough transform. 
   /*! The tables are used to speed up computation.
     This isn't strictly necessary, as this routine is not called very often, but it is
     not very difficult either (see \c isLocalMaximum). The function return the r-Hough 
     value of the best radius.
   */
  int findBestRadius (const Mat_<ushort>& sobelImg, int xC, int yC, int& bestR) const;
};


#endif
