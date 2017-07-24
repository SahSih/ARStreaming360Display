#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    //ubuntu command to check all usb cameras connected to OS and retrieve index OpenCV recognizes
    // ls -ltr /dev/video*
    // NOTE: the usb elp fisheye cams will have the time you connected them
    // /dev/video1 indicates the index OpenCV will recognize for camera is "1"
    // /dev/video2 indicates index "2" OpenCV will recognize for camera
    // if you have more cameras, then you can create more VideoCapture objects

    //instantiate two camera objects at index 1 and 2 (index can be different for every OS)
    VideoCapture cap1(1); // open the video camera no. 1
    VideoCapture cap2(2); // open the video camera no. 2

    //ubuntu command to verify OpenCV changes camera VideoCapture Format to correct value
    // v4l2-ctl -d /dev/video1 --all
    //Under "Format Video Capture", "Pixel Format" will show cameras current video stream format

    // set video codec for cameras from "YUYV" uncompressed to "MJPG" compression
    cap1.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M','J', 'P', 'G'));
    cap2.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M','J', 'P', 'G'));

    //ubuntu command to verify OpenCV changes FPS to correct value
    // v4l2-ctl -d /dev/video1 --all
    //Under "Streaming Parameters Video Capture", "Frames Per Second" will show cams current FPS
    
    // set all camera FPS to 30FPS (cameras default setting)
    cap1.set(CV_CAP_PROP_FPS, 30);
    cap2.set(CV_CAP_PROP_FPS, 30);

    //ubuntu command to verify OpenCV changes Frame Width/Height to correct value
    // v4l2-ctl -d /dev/video1 --all
    //Under "Format Video Capture", "Width/Height" will show cams current frame dimensions
 
    // set all camera frame Resolution to 640x480 (width = 640, height = 480)
    cap1.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    cap2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);


    // check all camera video codec output
    cout << " Camera 1 MJPG Codec : " << cap1.get(CV_CAP_PROP_FOURCC) << endl;
    cout << " Camera 2 MJPG Codec : " << cap2.get(CV_CAP_PROP_FOURCC) << endl;


    // check all camera FPS and display in console
    cout << " Camera 1 FPS : " << cap1.get(CV_CAP_PROP_FPS) << endl;
    cout << " Camera 2 FPS : " << cap2.get(CV_CAP_PROP_FPS) << endl;
 
    //Verify Camera 1 and 2 opened successfully, else emit error
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

    //grab all cameras frame dimensions
    double dWidth1 = cap1.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight1 = cap1.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    double dWidth2 = cap2.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight2 = cap2.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    //output cameras frame dimensions to console
    cout << "Frame size for CAM 1 : " << dWidth1 << " x " << dHeight1 << endl;
    cout << "Frame size for CAM 2 : " << dWidth2 << " x " << dHeight2 << endl;

    //create two windows for each cam: First window "Camera1_stream", second "Camera2_stream"
    namedWindow("Camera1_stream",CV_WINDOW_AUTOSIZE);
    namedWindow("Camera2_stream",CV_WINDOW_AUTOSIZE); 

    //loop to display frames per camera in their own separate window
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

	//Show frame1 in "Camera1_stream" window
        imshow("Camera1_stream", frame1);
	imshow("Camera2_stream", frame2);


        if ( (char) waitKey(30) == (char)27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break;
       }
    }
    return 0;
}
