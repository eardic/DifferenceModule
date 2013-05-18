/*
Emre Ardıç	
*/

#include <highgui.h>
#include <cxcore.h>
#include <fstream>
#include <cv.h>
#include <vector>
#include <iostream>
#include <ml.h>

using namespace cv;
using namespace std;

#define MINIMUM_GRAD_MAGNITUDE_FOR_ORIENTATION 0.05
#define PI 3.14159265

void GetHOGFeatures(const Mat &InputImage, Mat &Histogram);
Mat FindDifferences(const Mat &InputImage,const Mat &InputImage1);
void GetGradientMagAndOri(Mat InputImage, Mat & imageM, Mat & imageO);
void PlaceMark(const Mat& outputImage, const Mat& InputImage, Mat &diffImage);

bool CompareHistograms(const Mat &his1,const Mat &his2);
void ShowHistogram(const Mat &hist, string name);
Mat CropImage(const Mat &image,int x, int y, int size);

void BinarySegmentation(const Mat& inp, Mat& out);
void DilateAndErode(const Mat& inp, Mat& out);
void ContourOperation(Mat& inp, Mat& out, int threshold=100);

const int DIFFERENCE_TOLERANCE = 10;
const double ACCEPTABLE_PERCENT = 80;

int main(int argc, char* argv[])
{//C:/opencv/samples/cpp/

	try
	{
		Mat img1 = imread("lena.jpg",CV_LOAD_IMAGE_GRAYSCALE); // source image
		Mat img2 = imread("lena3.jpg",CV_LOAD_IMAGE_GRAYSCALE); // target image

		if(img1.data != NULL && img2.data != NULL)
		{// If images have been read safely,
			
			Mat diff=FindDifferences(img1,img2);;
			imshow("Difference", diff);

			// Binary segmentation
			Mat binSeg;
			BinarySegmentation(diff,binSeg);
			imshow("Binary Segmentation",binSeg);

			// Dilate - Erode
			Mat dilEro;
			DilateAndErode(binSeg,dilEro);
			imshow("DilateErode",dilEro);

			//Closed Contour
			Mat cont;
			ContourOperation(dilEro, cont);
			imshow("Contoured",cont);

		}
		else
		{
			cerr << "Failed to read images" << endl;
			char c;
			cin >> c;
		}

	}
	catch(Exception ex)
	{
		cout << ex.msg;
	}

	cvWaitKey(0);

	return 0;
}

void BinarySegmentation(const Mat& inp, Mat& out)
{
	const double THRESH = 0.1, MAX_VAL=255.0;
	threshold(inp, out, THRESH, MAX_VAL, CV_THRESH_BINARY);	



}

void DilateAndErode(const Mat& inp, Mat& out)
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

void ContourOperation(Mat& inp, Mat& out, int threshold)
{ 
	Mat canny_output;	
	vector<Vec4i> hierarchy;
        vector<vector<Point>> contours;
        
	inp.convertTo(inp,CV_8UC1);
								
	/// Detect edges using canny
	Canny( inp, canny_output, threshold, threshold*2, 3 );
	/// Find contours
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
			
	ofstream outFile("poly.txt");
	for(int i=0 ; i < contours.size() ; ++i){
		vector<Point> pArr = contours[i];
		for(int j=0 ; j < pArr.size() ; ++j){
			outFile << "X: " << pArr[j].x << " Y: " << pArr[j].y << endl;
		}
		outFile << endl;
	}
	outFile.close();

	/// Draw contours			
	Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	RNG rng(12345);
	Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );			
	for(unsigned int i = 0; i < contours.size(); i++ )
	{			
		drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, Point() );				
	}

	out = drawing.clone();
}

bool CompareHistograms(const Mat &his1,const Mat &his2)
{
	if(his1.cols != his2.cols){
		return false;
	}

	int nice=0, 
		h = 10,
		col_num = his1.cols - h;

	for(int i=0 ; i < col_num ; ++i)
	{
		int d2 = (his2.at<int>(0,i+h) - his2.at<int>(0,i)) / h;	
		int d1 = (his1.at<int>(0,i+h) - his1.at<int>(0,i)) / h;
		if( abs(d1-d2) < DIFFERENCE_TOLERANCE )
		{			
			++nice;
		}
	}
	
	cout << "Match Percent:" << (nice * (100.0 / col_num)) << endl;
	
	return (nice * (100.0 / col_num)) >= ACCEPTABLE_PERCENT;
}

void ShowHistogram(const Mat &hist,string name)
{
	int bins = 256; 

	Mat canvas;

	canvas = Mat::ones(125, bins, CV_8UC3);

	for (int j = 0, rows = canvas.rows; j < bins-1; j++)
	{
		line( canvas, Point(j, rows), 
			  Point(j, rows - (hist.at<int>(j) * rows)), 
			  Scalar(255,122,0), 1, 8, 0 );
	}

	imshow(name, canvas);
	
}

Mat CropImage(const Mat &image,int x, int y, int size)
{
	Mat newImage;
		
	newImage.create(size,size,DataType<float>::type);
	for(int r=0; r < size; ++r)
	{
		for(int c=0 ; c < size ; ++c)
		{
			newImage.at<float>(r,c) = image.at<float>(x+r,y+c);
		}
	}

	return newImage;
}

void PlaceMark(const Mat& outputImage, const Mat& InputImage, Mat &diffImage)
{
	// Marks differences
	diffImage = InputImage.clone();
	for(int r=0; r < InputImage.rows; r++)
	{
		for(int c=0 ; c < InputImage.cols; c++)
		{
			if(outputImage.at<float>(r,c) > 0.1)
			{
				diffImage.at<unsigned char>(r,c) = 255;
			}
		}
	}
}

void Histogram(Mat &h1, Mat &imageM, Mat& imageO, int x, int y , int blockSize)
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

Mat FindDifferences(const Mat &InputImage,const Mat &InputImage1)
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

	// Marks differences in target image
	//Mat diffImage;
	//PlaceMark(outputImage,InputImage1,diffImage);
	
	return outputImage;
}


void GetHOGFeatures(const Mat &InputImage, Mat &Histogram) 
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

void GetGradientMagAndOri(Mat InputImage, Mat & imageM, Mat & imageO) 
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