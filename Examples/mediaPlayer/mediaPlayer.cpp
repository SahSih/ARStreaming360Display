#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

// global variables
int g_slider_position = 0;
int g_run = 1, g_dontset = 0; //start out in single step mode
VideoCapture g_cap;

// allows user to advance video playback to new position
void onTrackbarSlide(int pos, void*)
{
	g_cap.set(cv::CAP_PROP_POS_FRAMES, pos);

  // After next new frame comes in, set program into single-step mode 
  // only if callback triggered by user click
	if(!g_dontset){
		g_run = 1; //pauses video when user clicks on trackbar to jump to new location
	}

	g_dontset = 0;
}

int main(int argc, char** argv) {
	namedWindow("mediaPlayer", cv::WINDOW_AUTOSIZE);
	g_cap.open(string(argv[1]));
	if(!g_cap.isOpened())
	{
		cout << "Cannot open video cam" << endl;
		return -1;
	}
	int frames = (int) g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	int tmpw = (int) g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int tmph = (int) g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	cout << "Video has " << frames << "frames of dimensions(" <<  tmpw << ", " << tmph << ") ." << endl;

  // Adds slider trackbar into video player to move around quickly within video
	createTrackbar("Position", "mediaPlayer", &g_slider_position, frames, onTrackbarSlide);

	Mat frame;
	for(;;){
		if(g_run != 0){
      // read in a frame
			g_cap >> frame;
			if(frame.empty()) break;

      // get current frame position
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
      // set, so next trackbar callback doesn't shift into single-step mode
			g_dontset = 1; 

      // update position of slider trackbar displayed to user
			setTrackbarPos("Position", "mediaPlayer", current_pos);
			imshow("mediaPlayer", frame);

      // decrement to stay in single-step mode 
      //or let video run depending on prior state set by user keypress
			g_run -= 1;
		}
		
    // User able to select from two modes: single step and run mode
		char c = (char) waitKey(10);
    // single step: jump to new location in video with trackbar and pause
		if(c == 's') //single step
		{
			g_run = 1;
			cout << "Single step, run = " << g_run << endl;
		}
    //run: continuous video mode
		if(c == 'r') //run mode
		{
			g_run = -1;
			cout << "Run mode, run = " << g_run << endl;
		}
		if(c == 27)
		{
			break;
		}
	}
	return 0;
}
