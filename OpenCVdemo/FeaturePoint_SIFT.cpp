#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

using namespace cv;
using namespace std;
int main() {
cout << "\n\nENTER file - Detect draw feature points from local picture file      \nENTER video - Detect draw feature points from a local video file               \nENTER live - Detect draw feature points from live streaming                   \n****************\n\n";
	string input;
	cin >> input;

	if(!input.compare("file")) {

	cout << "Loading from file...";
	Mat img = imread("road1.jpg");
	resize(img, img, Size(img.cols * 1, img.rows * 1),0,0, INTER_LINEAR);
	Mat img_copy;

	img.copyTo(img_copy);	
	Size gaussian_kernel = Size (9,9);
	GaussianBlur(img,img, gaussian_kernel, 9, 9);
	cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
	vector<KeyPoint> points;
	f2d->detect(img, points);
	drawKeypoints(img_copy, points, img_copy, Scalar(0, 0, 255));
	imshow("Feature_points", img_copy);
	waitKey(0);}


	if (!input.compare("video")){

	VideoCapture video_input;
	video_input.open ("video.MOV");
	Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();

	while(1) {
	
	Mat img;
	video_input >> img;
	resize(img, img, Size(img.cols * 1/4, img.rows * 1/4),0,0, INTER_LINEAR);
	Mat img_copy; 
	img.copyTo(img_copy);	
	Size gaussian_kernel = Size (9,9);
	GaussianBlur(img,img, gaussian_kernel, 9, 9);
  	//-- Step 1: Detect the keypoints:
  	vector<KeyPoint> points;    
  	f2d->detect( img, points );
	// detect feature point in the image by SIFT
	drawKeypoints(img_copy, points, img_copy, Scalar(0, 0, 255));
	// draw key point by the preset scalar
	imshow("Feature_Point", img_copy);
	// show the processed image
	waitKey(1);
	// Wait until any key is pressed
}
	}	

	if(!input.compare("live")) {
	
	cout << "Open external camera...";
	VideoCapture cam(0); 
	// select webcam, either built in or external
	Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
	// SIFT
	while(1) {
	Mat img;
	cam >> img;
	resize(img, img, Size(img.cols * 1, img.rows * 1),0,0, INTER_LINEAR);
	Mat img_copy; 
	img.copyTo(img_copy);	
	Size gaussian_kernel = Size (9,9);
	GaussianBlur(img,img, gaussian_kernel, 9, 9);
	// Import Image file/Read from file
    	//cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create()
  	//cv::Ptr<Feature2D> f2d = xfeatures2d::SURF::create();
  	//cv::Ptr<Feature2D> f2d = ORB::create();

  	//-- Step 1: Detect the keypoints:
  	vector<KeyPoint> points;    
  	f2d->detect( img, points );
	// detect feature point in the image by SIFT
	drawKeypoints(img_copy, points, img_copy, Scalar(0, 0, 255));
	// draw key point by the preset scalar
	imshow("Feature_Point", img_copy);
	// show the processed image
	waitKey(1);
	// Wait until any key is pressed
}
}
return 0;
}
