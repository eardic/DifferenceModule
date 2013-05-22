/*
Emre Ardıç	
*/


#include "img_diff.h"

#include <highgui.h>
#include <cxcore.h>
#include <fstream>
#include <cv.h>
#include <ml.h>

using namespace cv;
using namespace std;
using namespace ImgProcGyte;//ImgProc class

namespace ImgProcGyte
{
	void ImgProc::ConvexHullFitByDiff(string src_file, string target_file, string pointFile)
	{
		try
		{
			Mat img1 = imread(src_file,CV_LOAD_IMAGE_GRAYSCALE); // source image
			Mat img2 = imread(target_file,CV_LOAD_IMAGE_GRAYSCALE); // target image
	
			if(img1.data != NULL && img2.data != NULL)
			{// If images have been read safely,
		
				Mat diff=FindDifferences(img1,img2);				
				#ifdef DEBUG
				imshow("Difference", diff);
				#endif
				
				// Binary segmentation				
				Mat binSeg;
				BinarySegmentation(diff,binSeg);
				#ifdef DEBUG
				imshow("Binary Segmentation",binSeg);
				#endif
				
				// Dilate - Erode
				Mat dilEro;
				DilateAndErode(binSeg,dilEro);
				#ifdef DEBUG
				imshow("DilateErode",dilEro);
				#endif
				
				//Closed Contour				
				ContourOperation(dilEro, pointFile);
				cout << "\nImgProc :ConvexHull points are calculated successfully\n";
			}
			else
			{
				cerr << "Failed to read images" << endl;	
			}
		}
		catch(Exception ex)
		{
			cerr << "\nFail :" << ex.msg;
		}
	}

	void ImgProc::BinarySegmentation(const Mat& inp, Mat& out)
	{
		const double THRESH = 0.1, MAX_VAL=255.0;
		threshold(inp, out, THRESH, MAX_VAL, CV_THRESH_BINARY);	
	}

	void ImgProc::DilateAndErode(const Mat& inp, Mat& out)
	{	
		int dilate_dilation_size = 10, erode_dilation_size=dilate_dilation_size-2,
			dilation_type = MORPH_ELLIPSE;

		Mat dil_kernel = getStructuringElement( dilation_type,
												Size( 2*dilate_dilation_size + 1, 2*dilate_dilation_size+1 ),
												Point( dilate_dilation_size, dilate_dilation_size ) ),
			erode_kernel = getStructuringElement( dilation_type,
												  Size( 2*erode_dilation_size + 1, 2*erode_dilation_size+1 ),
												  Point( erode_dilation_size, erode_dilation_size ) );
		Mat dilated;
		dilate(inp,dilated,dil_kernel);
		erode(dilated,out,erode_kernel);
	}

	void ImgProc::ContourOperation(Mat& inp, string outFileName, int threshold)
	{
		Mat canny_output;
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		inp.convertTo(inp,CV_8UC1);
								
		/// Detect edges using canny
		Canny( inp, canny_output, threshold, threshold*2, 3 );
		/// Find contours
		findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	
		 /// Find the convex hull object for each contour
	   vector<vector<Point> >hull( contours.size() );
	   for(unsigned int i = 0; i < contours.size(); i++ )
	   {  
		   convexHull( Mat(contours[i]), hull[i], false ); 
	   }

		ofstream outFile(outFileName);
		for(unsigned int i=0 ; i < hull.size() ; ++i){
			vector<Point> pArr = hull[i];
			for(unsigned int j=0 ; j < pArr.size() ; ++j){
				outFile << "X: " << pArr[j].x << " Y: " << pArr[j].y << endl;
			}
			outFile << endl;
		}
		outFile.close();

		#ifdef DEBUG
		/// Draw contours			
		Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
		RNG rng(12345);
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );			
		for(unsigned int i = 0; i < contours.size(); i++ )
		{			
			drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, Point() );				
			drawContours( drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		}
		imshow("Contoured",drawing);
		#endif	
	}

	void ImgProc::Histogram(Mat &h1, Mat &imageM, Mat& imageO, int x, int y , int blockSize)
	{
		h1.create(1, 180, DataType<int>::type);
		// Clear Histogram
		for(int i=0; i < h1.cols; i++){
			h1.at<int>(0,i) = 0;
		}

		float stepSize = (float)(2.0*PI/(float)h1.cols);
		for (int r = x; r < x + blockSize ;r++)
		{
			for (int c = y; c < y + blockSize ;c++)
			{
				if (abs(imageM.at<float>(r,c)) > MINIMUM_GRAD_MAGNITUDE_FOR_ORIENTATION) 
				{
					float theta = imageO.at<float>(r,c); // between -pi and pi...
					theta += (float)PI;
					int count = (int)(theta / stepSize);
					if (count>=0 && count<h1.cols){ h1.at<int>(0,count) += 1; }
				}			
			}
		}

	}

	Mat ImgProc::FindDifferences(const Mat &InputImage,const Mat &InputImage1)
	{			
		const int COLS = InputImage.cols,
				  ROWS = InputImage.rows,
				  FRAME_SIZE = 3;

		Mat gradMag,gradOri,gradMag1,gradOri1;
		GetGradientMagAndOri(InputImage,gradMag,gradOri);
		GetGradientMagAndOri(InputImage1,gradMag1,gradOri1);
			
		Mat outputImage;	
		outputImage.create(ROWS, COLS,DataType<float>::type);	
	
		//Find differences
		float maxd = -1;
		for(int r=0; r < ROWS - FRAME_SIZE; r+=FRAME_SIZE)
		{
			for(int c=0 ; c < COLS - FRAME_SIZE ; c+=FRAME_SIZE)
			{
				Mat hist1,hist2;

				Histogram(hist1,gradMag,gradOri,r,c,FRAME_SIZE);
				Histogram(hist2,gradMag1,gradOri1,r,c,FRAME_SIZE);
			
				float d = 0;
				for (int i=0 ; i < hist1.cols; i++){
					float x = (float)hist1.at<int>(0,i) - (float)hist2.at<int>(0,i);
					d +=  x * x;
				}			
				outputImage.at<float>(r,c) = d;
				if ( d > maxd ) maxd = d;
			}
		}

		//Normalize
		fprintf(stderr, "maxd: %f\n", maxd);
		for(int r=0; r < ROWS - FRAME_SIZE; r+=FRAME_SIZE)
		{
			for(int c=0 ; c < COLS - FRAME_SIZE ; c+=FRAME_SIZE)
			{
				outputImage.at<float>(r,c) /= maxd;
			}
		}
			
		return outputImage;
	}


	void ImgProc::GetHOGFeatures(const Mat &InputImage, Mat &Histogram) 
	{
		Mat imageO, imageM;

		GetGradientMagAndOri(InputImage,imageM,imageO);

		int r,c;
		// form the histogram - will get rid of small magnitude orientations
		Histogram.create(1, 180, DataType<int>::type);
		for(c=0; c<Histogram.cols; c++) {
			Histogram.at<int>(0,c) = 0;
		}

		float stepSize = (float)(2.0*PI/(float)Histogram.cols);
		for (r=3;r<InputImage.rows-3;r++)
		{
			for (c=3;c<InputImage.cols-3;c++)
			{
				if (imageM.at<float>(r,c) > MINIMUM_GRAD_MAGNITUDE_FOR_ORIENTATION) 
				{
					float theta = imageO.at<float>(r,c); // between -pi and pi...
					theta += (float)PI;
					int count = (int)(theta / stepSize);
					if (count>=0 && count<Histogram.cols) Histogram.at<int>(0,count) += 1;
				}
			}
		}
	
	} // end

	void ImgProc::GetGradientMagAndOri(Mat InputImage, Mat & imageM, Mat & imageO) 
	{
		Mat gradH, gradV;

		Sobel(InputImage, gradH, DataType<float>::type, 1, 0, 3, 1.0, 0.0, BORDER_DEFAULT);
		Sobel(InputImage, gradV, DataType<float>::type, 0, 1, 3, 1.0, 0.0, BORDER_DEFAULT);

		imageM.create(InputImage.rows, InputImage.cols, DataType<float>::type);
		imageO.create(InputImage.rows, InputImage.cols, DataType<float>::type);

		// calculate magnitude and orientation images...
		float maxM = 0;
		int r, c;
		for (r=2;r<InputImage.rows-2;r++) {
			for (c=2;c<InputImage.cols-2;c++) {
				imageO.at<float>(r,c) = (float)(atan2(gradV.at<float>(r,c),gradH.at<float>(r,c)));
				imageM.at<float>(r,c) = gradH.at<float>(r,c)*gradH.at<float>(r,c) + gradV.at<float>(r,c)*gradV.at<float>(r,c);
				if (imageM.at<float>(r,c)>maxM) maxM = imageM.at<float>(r,c);
			}
		}
		// normalize magnitude image to 1...
		for (r=2;r<InputImage.rows-2;r++) {
			for (c=2;c<InputImage.cols-2;c++) {
				imageM.at<float>(r,c) /= maxM;
			}
		}

	} // end-getHOGFeatures1

}

// DRIVER PROGRAM
int main(int argc, char* argv[])
{//C:/opencv/samples/cpp/
	ImgProc::ConvexHullFitByDiff("lena.jpg","lena3.jpg","poly.txt");
	cvWaitKey(0);
	return 0;
}