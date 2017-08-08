#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 1. capture four cameras
int captureCameras(VideoCapture& cap1, VideoCapture& cap2);

// 2. Detect Keypoints using ORB Detection
void detectFeaturePointsViaOrb(Mat& frame1, Mat& frame2);

// 3. Calculate Descriptor via ORB Descriptor Extractor
void calculateDescriptors(Mat& greyFrame1, Mat& greyFrame2, vector<KeyPoint>& keypoints_frame1, vector<KeyPoint>& keypoints_frame2);

// 4. Use FlannBased Matcher to Match Descriptor Vectors
// 5. Use Homography to draw good matched features between frames
void matchDescriptorsViaFLANN(Mat& greyFrame1, Mat& greyFrame2, vector<KeyPoint>& keypoints_frame1, vector<KeyPoint>& keypoints_frame2, Mat& descriptors_frame1, Mat& descriptors_frame2);


int main(int argc, char* argv[])
{
	//instantiate two camera objects at index 1 and 2 (index can be different for every OS)
	VideoCapture cap1(1); // open the video camera no. 1
	VideoCapture cap2(2); // open the video camera no. 2

	captureCameras(cap1, cap2);
}

// 1. capture four cameras
int captureCameras(VideoCapture& cap1, VideoCapture& cap2) 
{
    // 1.1 set video codec for cameras from "YUYV" uncompressed to "MJPG" compression
    cap1.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M','J', 'P', 'G'));
    cap2.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M','J', 'P', 'G'));

    // 1.2 set all camera FPS to 30FPS (cameras default setting)
    cap1.set(CV_CAP_PROP_FPS, 30);
    cap2.set(CV_CAP_PROP_FPS, 30);

    // 1.3 set all camera frame dimension to 640x480 (width = 640, height = 480)
    cap1.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    cap2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    // 1.4 check all camera video codec output
    cout << " Camera 1 MJPG Codec : " << cap1.get(CV_CAP_PROP_FOURCC) << endl;
    cout << " Camera 2 MJPG Codec : " << cap2.get(CV_CAP_PROP_FOURCC) << endl;


    // 1.5 check all camera FPS and display in console
    cout << " Camera 1 FPS : " << cap1.get(CV_CAP_PROP_FPS) << endl;
    cout << " Camera 2 FPS : " << cap2.get(CV_CAP_PROP_FPS) << endl;
 
    //1.6 Verify Camera 1 and 2 opened successfully, else emit error
    if (!cap1.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam 1" << endl;
        return -1;
    }
    if (!cap2.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam 2" << endl;
        return -1;
    }

    //1.7 grab all cameras frame dimensions
    double dWidth1 = cap1.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight1 = cap1.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    double dWidth2 = cap2.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight2 = cap2.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    //1.8 output cameras frame dimensions to console
    cout << "Frame size for CAM 1 : " << dWidth1 << " x " << dHeight1 << endl;
    cout << "Frame size for CAM 2 : " << dWidth2 << " x " << dHeight2 << endl;

    //create two windows for each cam: First window "Camera1_stream", second "Camera2_stream"
    //namedWindow("Camera1_stream",CV_WINDOW_AUTOSIZE);
    //namedWindow("Camera2_stream",CV_WINDOW_AUTOSIZE); 

    //1. loop to display frames per camera in their own separate window
    while (1)
    {
	//frame1 created for camera1, frame2 for camera2
        Mat frame1, frame2;
	//read frame1 for camera1 and the return value to bSuccess1
        bool bSuccess1 = cap1.read(frame1);
	bool bSuccess2 = cap2.read(frame2);

	//Check whether frames from cameras are being read successfully, else output error
        if (!bSuccess1) //if not success, break loop
        {
             cout << "Cannot read a frame from video stream 1" << endl;
             break;
        }
	if (!bSuccess2) //if not success, break loop
        {
             cout << "Cannot read a frame from video stream 2" << endl;
             break;
        }

	detectFeaturePointsViaOrb(frame1, frame2);

	//Previously showed both camera frames 1 and 2. 
	//Now shows single frame with drawn corresponding feature points between frames 1 and 2
        //imshow("Camera1_stream", frame1);
	//imshow("Camera2_stream", frame2);

        if ( (char) waitKey(30) == (char)27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break;
       }
    }
	return 0;
}

// Use ORB Detector to compute various alternative descriptors

// 2. Detect Keypoints using ORB Detection (Oriented BRIEF)
// https://stackoverflow.com/questions/31835114/feature-detection-with-patent-free-descriptors
// http://docs.opencv.org/2.4/modules/features2d/doc/feature_detection_and_description.html#ORB%20:%20public%20Feature2D
void detectFeaturePointsViaOrb(Mat& frame1, Mat& frame2)
{
	// 2.1 Convert Mat to GrayScale
	//https://stackoverflow.com/questions/10344246/how-can-i-convert-a-cvmat-to-a-gray-scale-in-opencv
	Mat greyFrame1, greyFrame2;
	cvtColor(frame1, greyFrame1, cv::COLOR_BGR2GRAY);
	cvtColor(frame2, greyFrame2, cv::COLOR_BGR2GRAY);

	// 2.2 Detect the keypoints using FAST in pyramids 
	Ptr<FeatureDetector> detector = ORB::create();

	vector<KeyPoint> keypoints_frame1, keypoints_frame2;

	detector->detect(greyFrame1, keypoints_frame1);
	detector->detect(greyFrame2, keypoints_frame2);

	// 2.3 Calculator Descriptors using ORB Descriptor Extractor
	calculateDescriptors(greyFrame1, greyFrame2, keypoints_frame1, keypoints_frame2);
}

// 3. ORB Descriptor Extractor uses BRIEF to calculate descriptors

void calculateDescriptors(Mat& greyFrame1, Mat& greyFrame2, vector<KeyPoint>& keypoints_frame1, vector<KeyPoint>& keypoints_frame2)
{
	// 3.1 Create Ptr of DescriptorExtractor objects called extractor 
	Ptr<DescriptorExtractor> extractor = ORB::create();
	
	Mat descriptors_frame1, descriptors_frame2;

	// 3.2 Caculate Descriptors (Descriptions of key points)
	extractor->compute(greyFrame1, keypoints_frame1, descriptors_frame1);
	extractor->compute(greyFrame2, keypoints_frame2, descriptors_frame2);

	// 3.3 Match Feature Keypoints between frames using FlannBased Matcher
	matchDescriptorsViaFLANN(greyFrame1, greyFrame2, keypoints_frame1, keypoints_frame2, descriptors_frame1, descriptors_frame2);
}

// 4. Use FlannBased Matcher to Match Descriptor Vectors
// 5. Use Homography to draw good matched features between frames
void matchDescriptorsViaFLANN(Mat& greyFrame1, Mat& greyFrame2, vector<KeyPoint>& keypoints_frame1, vector<KeyPoint>& keypoints_frame2, Mat& descriptors_frame1, Mat& descriptors_frame2)
{
	//4.1 Create FlannBased Object and find matches between frames 
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	vector<DMatch> matches;
	matcher->match(descriptors_frame1, descriptors_frame2, matches);

	//4.2 Calculate max and min distance between keypoints
	double max_dist = 0, min_dist = 100;

	for(int i = 0; i < descriptors_frame1.rows; i++)
	{
		// compute distance between desc_frame1 and desc_frame2
		double dist = matches.at(i).distance;
		// if computed dist < min_dist, then computed dist is new min_dist
		if(dist < min_dist)
			min_dist = dist;
		// if computed dist < max_dist, then computed dist is new max_dist
		if(dist > max_dist)
			max_dist = dist;
	}
	
	cout << "-- Max dist: " << max_dist << endl;
	cout << "-- Min dist: " << min_dist << endl;

	//4.3 Draw only Good Matches (dist < 3*min_dist)
	vector<DMatch> good_matches;
	
	for(int i = 0; i < descriptors_frame1.rows; i++)
	{
		if(matches.at(i).distance < 3 * min_dist)
		{
			good_matches.push_back(matches.at(i));
		}
	}

	Mat frame_matches;

	drawMatches(greyFrame1, keypoints_frame1, greyFrame2, keypoints_frame2, good_matches, frame_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	// Localize priorFrame
	vector<Point2f> priorFrame;
	vector<Point2f> nextFrame;

	// 4.4 Get KeyPoints from Good Matches
	for(int i = 0; i < good_matches.size(); i++)
	{
		priorFrame.push_back(keypoints_frame1[good_matches.at(i).queryIdx].pt);
		nextFrame.push_back(keypoints_frame2[good_matches.at(i).trainIdx].pt);
	}

	// 5.1 Find Homography
	Mat H = findHomography(priorFrame, nextFrame, CV_RANSAC);

	// 5.2 Get Corners from PriorFrame
	vector<Point2f> priorFrame_corners(4);
	priorFrame_corners[0] = cvPoint(0,0);
	priorFrame_corners[1] = cvPoint(greyFrame1.cols,0);
	priorFrame_corners[2] = cvPoint(greyFrame1.cols, greyFrame1.rows);
	priorFrame_corners[3] = cvPoint(0,greyFrame1.rows);

	vector<Point2f> nextFrame_corners(4);
	// 5.3 Performs Perspective Matrix Transformation of Vectors
	perspectiveTransform(priorFrame_corners, nextFrame_corners, H);

	// 5.4 Draw Lines between Corner points between each frame
	line(frame_matches, nextFrame_corners[0] + Point2f(greyFrame1.cols, 0), nextFrame_corners[1] + Point2f(greyFrame1.cols, 0), Scalar(0, 255, 0), 4);
	line(frame_matches, nextFrame_corners[1] + Point2f(greyFrame1.cols, 0), nextFrame_corners[2] + Point2f(greyFrame1.cols, 0), Scalar(0, 255, 0), 4);
	line(frame_matches, nextFrame_corners[2] + Point2f(greyFrame1.cols, 0), nextFrame_corners[3] + Point2f(greyFrame1.cols, 0), Scalar(0, 255, 0), 4);
	line(frame_matches, nextFrame_corners[3] + Point2f(greyFrame1.cols, 0), nextFrame_corners[0] + Point2f(greyFrame1.cols, 0), Scalar(0, 255, 0), 4);

	//create window for detected matches between frames from cameras
	namedWindow("Good Matches & Object detection",CV_WINDOW_AUTOSIZE);

	// 5.5 Show detected matches
    	imshow("Good Matches & Object detection", frame_matches);
}
