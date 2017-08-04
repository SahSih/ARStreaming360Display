#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char* argv[]){
    //create window called "camera1_stream"
    cv::namedWindow("camera1_stream,", cv::WINDOW_AUTOSIZE);

    //Open the video camera no. 0
    cv::VideoCapture capture(0);

    //get frames per second of video
    double fps = capture.get(cv::CAP_PROP_FPS);
    cv::Size size(
        (int)capture.get(cv::CAP_PROP_FRAME_WIDTH),
        (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT)
    );

    //initialize VideoWriter object 
    cv::VideoWriter writer;
    writer.open("saveLiveStream.avi", CV_FOURCC('M','J','P','G'), fps, size);

    //if initialize writer fails, exit program
    if(!writer.isOpened())
    {
	std::cout << "ERROR: Failed to write the video" << std::endl;
	return -1;
    }

    cv::Mat bgr_frame;
    for(;;){
	//read a new frame from video
        capture >> bgr_frame;
        if(bgr_frame.empty()) break; //end if done
        
	//show frame in "camera1_stream" window
        cv::imshow("camera1_stream", bgr_frame);

	//write a frame into the file
        writer << bgr_frame;

	//wait for esc key pressed, then break loop
        char c = cv::waitKey(10);
        if(c == 27){ 
		std::cout << "esc key is pressed by user" << std::endl;
		break;
	};
    }//end for-loop
    capture.release();
    return 0;
}
