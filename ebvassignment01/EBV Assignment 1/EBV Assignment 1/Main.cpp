#include "Region.hpp"

//************ GUI



//! paint the extracted Intervals in \c rle onto \c img for displaying
/*! The color is chosen with a cyclic color table according to
\c .region
*/
void paintDecomposition(cv::Mat& img, const RegionSet& decomposition) {
	cv::Scalar color[6] = {
		CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), CV_RGB(255, 255, 0), CV_RGB(0, 0, 255), CV_RGB(255, 0, 255), CV_RGB(0, 255, 255)
	};
	for (int i = 0; i<(int)decomposition.rle.size(); i++) {
		line(img, cv::Point(decomposition.rle[i].xLo, decomposition.rle[i].y),
			cv::Point(decomposition.rle[i].xHi, decomposition.rle[i].y),
			color[decomposition.rle[i].region % 6]);
	}
}


//! Paint the center of gravity / intertial axis and labels for all regions onto \c img
/*! The routine paints a cross at the center with the lines of the cross corresponding
to the radii of an ellipse that would have the asme intertial properties.
*/
void paintFeatures(cv::Mat& img, const std::vector<Region>& features) {
	for (int i = 0; i<(int)features.size(); i++) {
		const Region* rg = &features[i];
		double c = cos(rg->mainAxis), s = sin(rg->mainAxis);
		// paint +/-largeLenge in direction (c,s) from the center of gravity
		line(img,
			cv::Point((int)(rg->centerX + rg->largeLength*c), (int)(rg->centerY + rg->largeLength*s)),
			cv::Point((int)(rg->centerX - rg->largeLength*c), (int)(rg->centerY - rg->largeLength*s)),
			CV_RGB(255, 255, 255));
		// paint +/-smallLength in direction (-s,c) orthogonal to (c,s) from the center of gravity
		line(img,
			cv::Point((int)(rg->centerX - rg->smallLength*s), (int)(rg->centerY + rg->smallLength*c)),
			cv::Point((int)(rg->centerX + rg->smallLength*s), (int)(rg->centerY - rg->smallLength*c)),
			CV_RGB(255, 255, 255));
		putText(img, rg->label, cv::Point((int)rg->centerX, (int)rg->centerY),
			cv::FONT_HERSHEY_PLAIN, 2, CV_RGB(255, 255, 255));
	}
}

void segmentImage(const char* filename){
	// Load the source image as a grayscale image for processing
	cv::Mat_<uchar> src = cv::imread(filename, 0); // 0 means load as grayscale
	// Load the same image as a color image for displaying and painting onto
	cv::Mat dst = cv::imread(filename, 1); // 1 means load as color (for displaying)

	if (!src.data) {
		printf("Image could not be loaded.\n");
		return;
	}

	// measure time
	int64 time1, time2;

	// Do the computer vision computation
	time1 = cv::getTickCount();
	RegionSet decomposition;
	int threshold = 80;
	int minLength = 2;
	decomposition.thresholdAndRLE(src, threshold, minLength);
	decomposition.groupRegions();
	std::vector<Region> region;
	Region::computeMoments(region, decomposition);
	for (int i = 0; i < region.size(); i++) {
		region[i].computeFeatures();
		region[i].classify();
	}
	time2 = cv::getTickCount();


	// measure time
	printf("Computation time %fs\n", (time2 - time1) / cv::getTickFrequency());

	// paint everything
	paintDecomposition(dst, decomposition);
	paintFeatures(dst, region);

	// show as a window in the GUI
	cv::imshow("Segmented Image", dst);

	printf("Click on the image and hit 'enter' to load another picture.\n");
	int key = cv::waitKey(0);

	// cv::imwrite ("result.png", dst);   // save the segmented and labeled result
	if (key == '\n')
		return;
}


int main() {

	cv::namedWindow("Segmented Image", CV_WINDOW_AUTOSIZE);
	std::string input;

	while (1){
		std::cout << "Enter the file name of the image or enter 'q' to quit." << std::endl;
		std::cin >> input;
		if (input != "q")
			segmentImage(input.c_str());
		else
			break;
	}

	return 0;
}