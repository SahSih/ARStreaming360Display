#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

// global variable keep trackbar slider position state
int g_slider_position = 0;
/** displays new frames when g_run != 0
* pos indicates num of frames displayed before stopping
* neg means system runs in continuous video mode
* when g_run = 1 and  user clicks on trackbar, 
*  the video is paused in new location (single-step state)
*/
int g_run = 1;
/**
* Allows us to update trackbar's position
* no more prgrammatic calls trigger single-step mode 
* when g_dontset = 1
*/
int g_dontset = 0; // start out in single step mode
// global variable so callback has access to capture object
VideoCapture g_cap;

/**
* callback advances video plackback when user slides trackbar
* @param pos 32 bit int is new trackbar position
* @param pointer to function
* @return void
*/
void onTrackbarSlide(int pos, void*)
{
	// advance video playback to new position
	//// set read position in units of frames
	g_cap.set(cv::CAP_PROP_POS_FRAMES, pos);

	// after next new frame comes in, program set to single-step mode
	// only if callback triggered by click
	if(!g_dontset){
		g_run = 1;
	}
	// else callback was called by main func and sets g_dontset

	g_dontset = 0;
}

int main(int argc, char** argv) {
	// create a named window
	namedWindow("mediaPlayer", cv::WINDOW_AUTOSIZE);
	// open videocam or video file
	// if file path given, then video file opened; else video cam
	// if video cam, then include "g_cap.open( string(argv[1]) );
	g_cap.open("SeanMalto2014EditHD.avi");
	if(!g_cap.isOpened())
	{
		cout << "Cannot open video cam" << endl;
		return -1;
	}
	// g_cap.get determines num of video frames, width, height of video images
	int frames = (int) g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	int tmpw = (int) g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int tmph = (int) g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	cout << "Video has " << frames << "frames of dimensions(" <<  tmpw << ", " << tmph << ") ." << endl;

	//create trackbar by indicating which window it should appear in
	// label that trackbar gets tagged wtih "Position"
	// window trackbar gets placed into "mediaPlayer"
	// frames num of frames in vdeo used to calibrate slider trackbar
	// &g_slider_position variable bound to the trackbar
	// onTrackbarSlide callback updates slider's position each time the slider is moved
	createTrackbar("Position", "mediaPlayer", &g_slider_position, frames, onTrackbarSlide);

	Mat frame;
	for(;;){
		// if frames are being displayed
		if(g_run != 0){
			// read video frame
			g_cap >> frame;
			if(frame.empty()) break;

			// get current position in video
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			// set g_dontset, so next trackbar callback doesn't trigger single-step mode
			g_dontset = 1;
			
			// invoke trackbar callback to update position of slider trackbar on display
			setTrackbarPos("Position", "mediaPlayer", current_pos);
			// display vdeo frame
			imshow("mediaPlayer", frame);

			// g_run dec to stay in single-step mode
			// or lets video run depend on prior state set by user keypress
			g_run -= 1;
		}
		
		char c = (char) waitKey(10);
		if(c == 's') // single-step mode
		{
			// allow reads of single frame
			g_run = 1;
			cout << "Single step, run = " << g_run << endl;
		}
		if(c == 'r') // continuous video mode
		{
			// dec g_run and leaves it neg for any conceivable video size
			g_run = -1;
			cout << "Run mode, run = " << g_run << endl;
		}
		if(c == 27) // esc pressed
		{
			// terminate program
			break;
		}
	}
	return 0;
}
