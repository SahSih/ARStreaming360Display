//OpenFrames
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <tbb/concurrent_queue.h>
#include <opencv2/videoio.hpp>
#include "opencv2/opencv_modules.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"

#define ENABLE_LOG 1
#define LOG(msg) std::cout << msg
#define LOGLN(msg) std::cout << msg << std::endl

using namespace std;
using namespace cv;
using namespace tbb;
using namespace cv::detail;

/**
*
*  Part 1: User Interaction with Program via CLI
*
*/

/*Help Message that shows how to use CLI to interact with program*/
static void printProgUsage();

/*Default CLI Args*/

//this holds video files
vector<string> vidNames;

//Check commands inputted to the program via CLI
static int parseCmdArgs(int argc, char** argv);

/**
*
*  Part 2: Open/Close Multi-Cameras or Video Files via Thread Building Block
*	(TBB)
*
*  Modified Putu Yuwono Kusmawan Multi-threaded Code to interact with
*  Multi-VideoFile Capture
*  https://putuyuwono.wordpress.com/2015/05/29/multi-thread-multi-camera-capture-using-opencv/
*/

//initialize and start camera capturing process(es)
void startMultiCapture(vector<VideoCapture*> &camCap, vector<concurrent_queue<Mat>*> &frameQueue, vector<thread*> &cameraThrd, int &videoIndex);

//release all camera capture resources(s)
void stopMultiCapture(vector<VideoCapture*> &camCap, int &videoIndex);

//main camera capturing process that'll be done by thread(s)
void captureFrame(vector<VideoCapture*> &camCap, vector<concurrent_queue<Mat>*> &frmQueue, int videoIndex);

//display all cameras in GUI windows
void displayMultiCams(vector<concurrent_queue<Mat>*> &frameQueue, int &videoIndex);

/**
*
*  Main: Get total video files, cameras from parseCmdArgs
*	 Store total number of video files into videoIndex
*	 Create vector of VideoCapture ptrs called camCapture
*	 Create vector of concurrent_queue<Mat> ptrs called frameQueue
*	     for every frame, it must have a designated concurrent_queue<Mat>*
*	 Create vector of thread ptrs called cameraThread
*
*	 Start Multi-Camera Capturing Process(es)
*	 Display Multi-Cameras in their own windows
*	 Stop Multi-Camera Capturing Process(es)
*
*/

int main( int argc, char** argv )
{
#if ENABLE_LOG
	int64 appStartTime = getTickCount();
#endif

#if 0
	cv::setBreakOnError(true);
#endif
	int retval = parseCmdArgs(argc, argv);
	if(retval)
	{
		return retval;
	}

	int videoIndex = static_cast<int>(vidNames.size());
	if(videoIndex < 1)
	{
		LOGLN("Need more videos");
		return -1;
	}

	//this holds OpenCV VideoCapture pointers
	vector<VideoCapture*> camCapture;
	//this holds queue(s) which hold frames from each camera
	vector<concurrent_queue<Mat>*> frameQueue;
	//this holds thread(s) which run the camera capture process
	vector<thread*> cameraThread;

	//initialize and start camera capturing process(es)
	startMultiCapture(camCapture, frameQueue, cameraThread, videoIndex);	

	//display all cameras in GUI windows
	displayMultiCams(frameQueue, videoIndex);

	//release all camera capture resources(s)
	stopMultiCapture(camCapture, videoIndex);
	return 0;
}

//Help Message on how to interact with program
static void printProgUsage()
{
	cout <<
		"Rotation model video stitcher\n\n"
		"Stitching_detailed vid1 vid2 [...vidN] [flags]\n\n";
}

//Check commands inputted to the program via CLI, enable user interaction
static int parseCmdArgs(int argc, char** argv)
{
	if(argc == 1)
	{
		printProgUsage();
		return -1;
	}
	for(int i = 1; i < argc; ++i)
	{
		if(string(argv[i]) == "--help" || string(argv[i]) == "--h")
		{
			printProgUsage();
			return -1;
		}
		else
		{
			vidNames.push_back(argv[i]);
		}
	}
	return 0;
}

//initialize and start camera capturing process(es)
void startMultiCapture(vector<VideoCapture*> &camCap, vector<concurrent_queue<Mat>*> &frameQueue, vector<thread*> &cameraThrd, int &videoIndex)
{
	VideoCapture *capture;
	thread *camThrd;
	concurrent_queue<Mat> *frmQueue;
	cout << "Video Index: " << videoIndex << endl;
	for(int i = 0; i < videoIndex; ++i)
	{
		//Make VideoCapture instance
		capture = new VideoCapture(vidNames.at(i));
		if(!capture->isOpened())
		{
			cout << "Failed to open file: " << vidNames.at(i) << endl;
			break;
		}
		cout << "Video File Setup: " << (vidNames.at(i)) << endl;
		//Insert VideoCapture to the vector
		camCap.push_back(capture);
		//Make thread instance
		camThrd = new thread(captureFrame, ref(camCap), ref(frameQueue), i);
		//Insert thead to vector
		cameraThrd.push_back(camThrd);
		//Make queue instance
		frmQueue = new concurrent_queue<Mat>;
		//Insert queue to the vector of frame objects
		frameQueue.push_back(frmQueue);
	}
}

//main camera capturing process that'll be done by thread(s)
void captureFrame(vector<VideoCapture*> &camCap, vector<concurrent_queue<Mat>*> &frmQueue, int videoIndex)
{
	cout << "Inside captureFrame:" << endl;
	cout << "videoIndex = " << videoIndex << endl;
	VideoCapture *capture = camCap.at(videoIndex);
	while(true)
	{
		Mat frame;
		//Grab frame from camera capture, if successful
		if((*capture).read(frame))
		{
			cout << "Going to push in frame from vidIndex: " << videoIndex << endl;
			//Insert frame to the queue
			frmQueue.at(videoIndex)->push(frame);
		}
		//release frame resource
		frame.release();
	}
}

//release all camera capture resources(s)
void stopMultiCapture(vector<VideoCapture*> &camCap, int &videoIndex)
{
	VideoCapture *capture;
	for(int i = 0; i < videoIndex; ++i)
	{
		capture = camCap.at(i);
		if(capture->isOpened())
		{
			//Release VideoCapture resource
			capture->release();
			cout << "Capture " << i << " released" << endl;
		}
	}
}

//display all cameras in GUI windows
void displayMultiCams(vector<concurrent_queue<Mat>*> &frameQueue, int &videoIndex)
{
		//loop while key not pressed
	while(waitKey(20) != 27)
	{
		//Retrieve frames from each video capture thread
		for(int i = 0; i < videoIndexTotal; ++i)
		{
			Mat frame;
			//Pop frame from queue and check if frame is valid
			if(frameQueue.at(i)->try_pop(frame))
			{
				//Show frame
				imshow(vidNames.at(i), frame);
			}
		}	
	}
}
