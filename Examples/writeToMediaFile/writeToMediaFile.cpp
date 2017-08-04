#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char* argv[]){

    //Open the video camera no. 0
    cv::VideoCapture cap1(0); // open the video camera no. 1
    cv::VideoCapture cap2(1); // open the video camera no. 2
    cv::VideoCapture cap3(2); // open the video camera no. 3
    cv::VideoCapture cap4(3); // open the video camera no. 4

    //get frames per second of video
    double fps = cap1.get(cv::CAP_PROP_FPS);
    cv::Size size(
        (int)cap1.get(cv::CAP_PROP_FRAME_WIDTH),
        (int)cap1.get(cv::CAP_PROP_FRAME_HEIGHT)
    );
	
    // create four windows for each writer
    namedWindow("northCamera",CV_WINDOW_AUTOSIZE);
    namedWindow("eastCamera",CV_WINDOW_AUTOSIZE); 
    namedWindow("southCamera",CV_WINDOW_AUTOSIZE);
    namedWindow("westCamera",CV_WINDOW_AUTOSIZE);

    //initialize VideoWriter object 
    cv::VideoWriter writer1, writer2, writer3, writer4;
    writer1.open("saveLiveStreamNorth.avi", CV_FOURCC('M','J','P','G'), fps, size);
    writer2.open("saveLiveStreamEast.avi", CV_FOURCC('M','J','P','G'), fps, size);
    writer3.open("saveLiveStreamSouth.avi", CV_FOURCC('M','J','P','G'), fps, size);
    writer4.open("saveLiveStreamWest.avi", CV_FOURCC('M','J','P','G'), fps, size);
	
    //if initialize writer fails, exit program
    if(!writer1.isOpened() && !writer2.isOpened() && !writer3.isOpened() && !writer4.isOpened())
    {
	std::cout << "ERROR: Failed to write the video" << std::endl;
	return -1;
    }

    cv::Mat frame1, frame2, frame3, frame4;
    for(;;){
	//read a new frame from video
        cap1 >> frame1;
	cap2 >> frame2;
	cap3 >> frame3;
	cap4 >> frame4;
        if(frame1.empty()) break; //end if done
        if(frame2.empty()) break;
	if(frame3.empty()) break;
	if(frame4.empty()) break;
	    
	//show frame in "camera1_stream" window
        cv::imshow("northCamera", frame1);
	cv::imshow("eastCamera", frame2);
	cv::imshow("southCamera", frame3);
	cv::imshow("westCamera", frame4);

	//write a frame into the file
        writer1 << frame1;
	writer2 << frame2;
	writer3 << frame3;
	writer4 << frame4;

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
