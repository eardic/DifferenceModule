
#ifndef IMG_DIFF_H
#define IMG_DIFF_H

//#define DEBUG

#include <cv.h>
using namespace cv;

namespace ImgProcGyte
{	
	// Constants
	const int DIFFERENCE_TOLERANCE = 10;
	const double ACCEPTABLE_PERCENT = 80;
	const double PI = 3.14159265;
	const double MINIMUM_GRAD_MAGNITUDE_FOR_ORIENTATION = 0.05;

	class ImgProc
	{
		public:
			// Gets names of images to find differences between them,and name of output file
			// ConvexHull Points are written to pointFile.
			static void ConvexHullFitByDiff(string img1, string img2, string pointFile);

		private :
			// Helper methods.
			static void GetHOGFeatures(const Mat &InputImage, Mat &Histogram);
			static Mat FindDifferences(const Mat &InputImage,const Mat &InputImage1);
			static void GetGradientMagAndOri(Mat InputImage, Mat & imageM, Mat & imageO);
			static void BinarySegmentation(const Mat& inp, Mat& out);
			static void DilateAndErode(const Mat& inp, Mat& out);
			static void ContourOperation(Mat& inp, string outFileName, int threshold=100);	
			static void Histogram(Mat &h1, Mat &imageM, Mat& imageO, int x, int y , int blockSize);
	};

}

#endif