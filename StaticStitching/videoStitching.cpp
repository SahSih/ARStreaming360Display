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
bool preview = false;
bool tryCuda = false;
double workMegapix = 0.6;
double seamMegapix = 0.1;
double composeMegapix = -1;
float confThresh = 1.f;
string featuresType = "orb";
string matcherType = "homography";
string estimatorType = "homography";
string baCostFunc = "ray";
string baRefineMask = "xxxxx";
bool doWaveCorrect = true;
WaveCorrectKind wave_correct = detail::WAVE_CORRECT_HORIZ;
bool saveGraph = false;
string saveGraphTo;
string warpType = "spherical";
int exposCompType = ExposureCompensator::GAIN_BLOCKS;
float matchConf = 0.3f;
string seamFindType = "gc_color";
int blendType = Blender::MULTI_BAND;
int timelapseType = Timelapser::AS_IS;
float blendStrength = 5;
string resultName = "result.jpg";
bool timelapse = false;
int rangeWidth = -1;

double seamWorkAspect = 1;
double workScale = 1, seamScale = 1, composeScale = 1;
bool isWorkScaleSet = false, isSeamScaleSet = false, isComposeScaleSet = false;

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
void startMultiCapture(vector<VideoCapture*> &camCap, vector<concurrent_queue<ImageFeatures>*> &featuresQueue, vector<concurrent_queue<Mat>*> &frameQueue, Ptr<FeaturesFinder> &finder, vector<Size> &fullFrameSizes, vector<thread*> &cameraThrd, int &numVideos);

//release all camera capture resources(s)
void stopMultiCapture(vector<VideoCapture*> &camCap, int &numVideos);

//main camera capturing process that'll be done by thread(s)
void captureFrame(vector<VideoCapture*> &camCap, vector<concurrent_queue<ImageFeatures>*> &featuresQueue, vector<concurrent_queue<Mat>*> &frmQueue, Ptr<FeaturesFinder> &finder, vector<Size> &fullFrameSizes, int numVideos);

/*
//detect feature keypoints per video frame
void detectFeaturesPerVideoFrames(Ptr<FeaturesFinder> &finder, vector<ImageFeatures> &features, vector<Size> &fullFrameSizes, Mat &fullFrame, Mat &frame, int &i);
*/

//display all videos in GUI windows
void displayMultiCams(vector<concurrent_queue<Mat>*> &frameQueue, int &numVideos);

//display keypoints of all videos in GUI windows
void displayFeaturesPerCam(vector<concurrent_queue<ImageFeatures>*> &featuresQueue, vector<concurrent_queue<Mat>*> &frameQueue, int &numVideos);

/**
*
*  Main: Get total video files, cameras from parseCmdArgs
*	 Store total number of video files into numVideos
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
	
	int numVideos = static_cast<int>(vidNames.size());
	if(numVideos < 1)
	{
		LOGLN("Need more videos");
		return -1;
	}

/**
*	Initialize VideoCapture for startMultiCapture
*/
	//this holds OpenCV VideoCapture pointers
	vector<VideoCapture*> camCapture;
	//this holds queue(s) which hold frames from each camera
	vector<concurrent_queue<Mat>*> frameQueue;
	//this holds thread(s) which run the camera capture process
	vector<thread*> cameraThread;

/**
*	Initialize FeatureDetection for Detecting Features Per Video Frame
*/

	LOGLN("Finding features...");
#if ENABLE_LOG
	int64 t = getTickCount();
#endif
	Ptr<FeaturesFinder> finder;
	if(featuresType == "surf")
    	{
#ifdef HAVE_OPENCV_XFEATURES2D
		if (tryCuda && cuda::getCudaEnabledDeviceCount() > 0)
			finder = makePtr<SurfFeaturesFinderGpu>();
		else
#endif
			finder = makePtr<SurfFeaturesFinder>();
    	}
	else if (featuresType == "orb")
	{
        	finder = makePtr<OrbFeaturesFinder>();
	}
	else
	{
        	cout << "Unknown 2D features type: '" << featuresType << "'.\n";
        	return -1;
	}
	//Already have full_img and img based on frameQueue and frame
	//Frames Per Video Duration (FPVD)
	//Features Per Video Frames, Frames Per Video and fullFrameSizes
	//	are calculated based on FPS, SEC_TO_MIN and MINUTES running
	//Each video file is approximately 2 minutes
	//video files were recorded with ELP 170 Fisheye that output 30 FPS
	int FPS = 30;
	int SEC_TO_MIN = 60;
	int MINUTES = 2;	//Estimated Time Video Plays
	int FPVD = FPS * SEC_TO_MIN * MINUTES;
	//this holds queue(s) which holds all frames keypoint features per camera
	vector<concurrent_queue<ImageFeatures>*> featuresQueue;
	//Estimate num of frames to allocate for each video using FPVD calc
	vector<Size> fullFrameSizes(FPVD);


	//initialize and start camera video stitching process(es)
	startMultiCapture(camCapture, featuresQueue, frameQueue, finder, fullFrameSizes, cameraThread, numVideos);	

LOGLN("Finding features, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");

	//display features in frames per video
	displayFeaturesPerCam(featuresQueue, frameQueue, numVideos);

	//display all cameras in GUI windows
	//displayMultiCams(frameQueue, numVideos);

	//release all camera capture resources(s)
	stopMultiCapture(camCapture, numVideos);
	return 0;
}

//Help Message on how to interact with program
static void printProgUsage()
{
	cout <<
		"Rotation model video stitcher\n\n"
		"Stitching_detailed vid1 vid2 [...vidN] [flags]\n\n"
		"Flags:\n"
		"  --preview\n"
		"	Run stitching in the preview mode. Works faster than usual mode,\n"
		"	but output video will have lower resoution.\n"
		"  --try_cuda (yes|no)\n"
		"	Try to use CUDA. The default value is 'no'. All default values\n"
		"	are for CPU mode.\n"
		"\nMotion Estimation Flags:\n"
		"  --work_megapix <float>\n"
        	"      Resolution for image registration step. The default is 0.6 Mpx.\n"
        	"  --features (surf|orb)\n"
        	"      Type of features used for images matching. The default is surf.\n"
        	"  --matcher (homography|affine)\n"
        	"      Matcher used for pairwise image matching.\n"
        	"  --estimator (homography|affine)\n"
        	"      Type of estimator used for transformation estimation.\n"
        	"  --match_conf <float>\n"
        	"      Confidence for feature matching step. The default is 0.65 for surf and 0.3 for orb.\n"
        	"  --conf_thresh <float>\n"
        	"      Threshold for two images are from the same panorama confidence.\n"
        	"      The default is 1.0.\n"
        	"  --ba (no|reproj|ray|affine)\n"
        	"      Bundle adjustment cost function. The default is ray.\n"
        	"  --ba_refine_mask (mask)\n"
        	"      Set refinement mask for bundle adjustment. It looks like 'x_xxx',\n"
        	"      where 'x' means refine respective parameter and '_' means don't\n"
        	"      refine one, and has the following format:\n"
        	"      <fx><skew><ppx><aspect><ppy>. The default mask is 'xxxxx'. If bundle\n"
        	"      adjustment doesn't support estimation of selected parameter then\n"
        	"      the respective flag is ignored.\n"
        	"  --wave_correct (no|horiz|vert)\n"
        	"      Perform wave effect correction. The default is 'horiz'.\n"
        	"  --save_graph <file_name>\n"
        	"      Save matches graph represented in DOT language to <file_name> file.\n"
        	"      Labels description: Nm is number of matches, Ni is number of inliers,\n"
        	"      C is confidence.\n"
		"\nCompositing Flags:\n"
		"  --warp (affine|plane|cylindrical|spherical|fisheye|stereographic|compressedPlaneA2B1|compressedPlaneA1.5B1|compressedPlanePortraitA2B1|compressedPlanePortraitA1.5B1|paniniA2B1|paniniA1.5B1|paniniPortraitA2B1|paniniPortraitA1.5B1|mercator|transverseMercator)\n"
		"      Warp surface type. The default is 'spherical'.\n"
		"  --seam_megapix <float>\n"
		"      Resolution for seam estimation step. The default is 0.1 Mpx.\n"
		"  --seam (no|voronoi|gc_color|gc_colorgrad)\n"
		"      Seam estimation method. The default is 'gc_color'.\n"
		"  --compose_megapix <float>\n"
		"      Resolution for compositing step. Use -1 for original resolution.\n"
		"      The default is -1.\n"
		"  --expos_comp (no|gain|gain_blocks)\n"
		"      Exposure compensation method. The default is 'gain_blocks'.\n"
		"  --blend (no|feather|multiband)\n"
		"      Blending method. The default is 'multiband'.\n"
		"  --blend_strength <float>\n"
		"      Blending strength from [0,100] range. The default is 5.\n"
		"  --output <result_img>\n"
		"      The default is 'result.jpg'.\n"
		"  --timelapse (as_is|crop) \n"
		"      Output warped images separately as frames of a time lapse movie, with 'fixed_' prepended to input file names.\n"
		"  --rangewidth <int>\n"
		"      uses range_width to limit number of images to match with.\n";
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
		if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
		{
		    printProgUsage();
		    return -1;
		}
		else if (string(argv[i]) == "--preview")
		{
		    preview = true;
		}
		else if (string(argv[i]) == "--try_cuda")
		{
		    if (string(argv[i + 1]) == "no")
		        tryCuda = false;
		    else if (string(argv[i + 1]) == "yes")
		        tryCuda = true;
		    else
		    {
		        cout << "Bad --try_cuda flag value\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--work_megapix")
		{
		    workMegapix = atof(argv[i + 1]);
		    i++;
		}
		else if (string(argv[i]) == "--seam_megapix")
		{
		    seamMegapix = atof(argv[i + 1]);
		    i++;
		}
		else if (string(argv[i]) == "--compose_megapix")
		{
		    composeMegapix = atof(argv[i + 1]);
		    i++;
		}
		else if (string(argv[i]) == "--result")
		{
		    resultName = argv[i + 1];
		    i++;
		}
		else if (string(argv[i]) == "--features")
		{
		    featuresType = argv[i + 1];
		    if (featuresType == "orb")
		        matchConf = 0.3f;
		    i++;
		}
		else if (string(argv[i]) == "--matcher")
		{
		    if (string(argv[i + 1]) == "homography" || string(argv[i + 1]) == "affine")
		        matcherType = argv[i + 1];
		    else
		    {
		        cout << "Bad --matcher flag value\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--estimator")
		{
		    if (string(argv[i + 1]) == "homography" || string(argv[i + 1]) == "affine")
		        estimatorType = argv[i + 1];
		    else
		    {
		        cout << "Bad --estimator flag value\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--match_conf")
		{
		    matchConf = static_cast<float>(atof(argv[i + 1]));
		    i++;
		}
		else if (string(argv[i]) == "--conf_thresh")
		{
		    confThresh = static_cast<float>(atof(argv[i + 1]));
		    i++;
		}
		else if (string(argv[i]) == "--ba")
		{
		    baCostFunc = argv[i + 1];
		    i++;
		}
		else if (string(argv[i]) == "--ba_refine_mask")
		{
		    baRefineMask = argv[i + 1];
		    if (baRefineMask.size() != 5)
		    {
		        cout << "Incorrect refinement mask length.\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--wave_correct")
		{
		    if (string(argv[i + 1]) == "no")
		        doWaveCorrect = false;
		    else if (string(argv[i + 1]) == "horiz")
		    {
		        doWaveCorrect = true;
		        wave_correct = detail::WAVE_CORRECT_HORIZ;
		    }
		    else if (string(argv[i + 1]) == "vert")
		    {
		        doWaveCorrect = true;
		        wave_correct = detail::WAVE_CORRECT_VERT;
		    }
		    else
		    {
		        cout << "Bad --wave_correct flag value\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--save_graph")
		{
		    saveGraph = true;
		    saveGraphTo = argv[i + 1];
		    i++;
		}
		else if (string(argv[i]) == "--warp")
		{
		    warpType = string(argv[i + 1]);
		    i++;
		}
		else if (string(argv[i]) == "--expos_comp")
		{
		    if (string(argv[i + 1]) == "no")
		        exposCompType = ExposureCompensator::NO;
		    else if (string(argv[i + 1]) == "gain")
		        exposCompType = ExposureCompensator::GAIN;
		    else if (string(argv[i + 1]) == "gain_blocks")
		        exposCompType = ExposureCompensator::GAIN_BLOCKS;
		    else
		    {
		        cout << "Bad exposure compensation method\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--seam")
		{
		    if (string(argv[i + 1]) == "no" ||
		        string(argv[i + 1]) == "voronoi" ||
		        string(argv[i + 1]) == "gc_color" ||
		        string(argv[i + 1]) == "gc_colorgrad" ||
		        string(argv[i + 1]) == "dp_color" ||
		        string(argv[i + 1]) == "dp_colorgrad")
		        seamFindType = argv[i + 1];
		    else
		    {
		        cout << "Bad seam finding method\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--blend")
		{
		    if (string(argv[i + 1]) == "no")
		        blendType = Blender::NO;
		    else if (string(argv[i + 1]) == "feather")
		        blendType = Blender::FEATHER;
		    else if (string(argv[i + 1]) == "multiband")
		        blendType = Blender::MULTI_BAND;
		    else
		    {
		        cout << "Bad blending method\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--timelapse")
		{
		    timelapse = true;

		    if (string(argv[i + 1]) == "as_is")
		        timelapseType = Timelapser::AS_IS;
		    else if (string(argv[i + 1]) == "crop")
		        timelapseType = Timelapser::CROP;
		    else
		    {
		        cout << "Bad timelapse method\n";
		        return -1;
		    }
		    i++;
		}
		else if (string(argv[i]) == "--rangewidth")
		{
		    rangeWidth = atoi(argv[i + 1]);
		    i++;
		}
		else if (string(argv[i]) == "--blend_strength")
		{
		    blendStrength = static_cast<float>(atof(argv[i + 1]));
		    i++;
		}
		else if (string(argv[i]) == "--output")
		{
		    resultName = argv[i + 1];
		    i++;
		}
		else
		    vidNames.push_back(argv[i]);
	}
	if (preview)
	{
		composeMegapix = 0.6;
	}
	return 0;
}

//initialize and start video stitching process(es)
void startMultiCapture(vector<VideoCapture*> &camCap, vector<concurrent_queue<ImageFeatures>*> &featuresQueue, vector<concurrent_queue<Mat>*> &frameQueue, Ptr<FeaturesFinder> &finder, vector<Size> &fullFrameSizes, vector<thread*> &cameraThrd, int &numVideos)
{
	VideoCapture *capture;
	thread *camThrd;
	concurrent_queue<Mat> *frmQueue;
	concurrent_queue<ImageFeatures> *ftrsQueue;
	cout << "Video Index: " << numVideos << endl;
	for(int i = 0; i < numVideos; ++i)
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
		camThrd = new thread(captureFrame, ref(camCap), ref(featuresQueue), ref(frameQueue), ref(finder), ref(fullFrameSizes), i);
		//Insert thead to vector
		cameraThrd.push_back(camThrd);
		//Make queue instance
		frmQueue = new concurrent_queue<Mat>;
		ftrsQueue = new concurrent_queue<ImageFeatures>;
		//Insert queue to the vector of frame objects
		frameQueue.push_back(frmQueue);
		featuresQueue.push_back(ftrsQueue);
	}
}

//main camera capturing process that'll be done by thread(s)
void captureFrame(vector<VideoCapture*> &camCap, vector<concurrent_queue<ImageFeatures>*> &featuresQueue, vector<concurrent_queue<Mat>*> &frmQueue, Ptr<FeaturesFinder> &finder, vector<Size> &fullFrameSizes, int numVideos)
{
	cout << "Inside captureFrame:" << endl;
	cout << "numVideos = " << numVideos << endl;
	VideoCapture *capture = camCap.at(numVideos);
	int i = 0;
	Mat fullFrame, frame;
	ImageFeatures features;
	while(true)
	{
		//Grab frame from camera capture, if successful
		if((*capture).read(fullFrame))
		{
			cout << "Going to push in frame from vidIndex: " << numVideos << endl;
			//feature Detection on frame before it gets pushed to frameQueue
			//detectFeaturesPerVideoFrame(finder, features, fullFrameSizes, fullFrame, frame, i);

			fullFrameSizes[i] = fullFrame.size();
			if(fullFrame.empty())
			{
				LOGLN("Can't open frame " << vidNames.at(numVideos));
				break;
			}
			if(workMegapix < 0)
			{
				frame = fullFrame;
				workScale = 1;
				isWorkScaleSet = true;
			}
			else
			{
				if(!isWorkScaleSet)
				{
					workScale = min(1.0, sqrt(workMegapix * 1e6 / fullFrame.size().area()));
					isWorkScaleSet = true;
				}
				resize(fullFrame, frame, Size(), workScale, workScale);
			}
			if(!isSeamScaleSet)
			{
				seamScale = min(1.0, sqrt(seamMegapix * 1e6 / fullFrame.size().area()));
				seamWorkAspect = seamScale / workScale;
				isSeamScaleSet = true;
			}
			(*finder)(frame, features);
			features.img_idx = i;
			LOGLN("Features in frame #" << i+1 << ": " << features.keypoints.size());
			resize(fullFrame, frame, Size(), seamScale, seamScale);

			//Insert frame to the queue
			//frmQueue also takes the place of Mat images(FPVD)
			featuresQueue.at(numVideos)->push(features);
			frmQueue.at(numVideos)->push(frame);
			i++;
		}
		//release frame resource
		frame.release();
	}
	finder->collectGarbage();
	fullFrame.release();
}

/*
//Detects Features Per Video File Frames
void detectFeaturesPerVideoFrames(Ptr<FeaturesFinder> &finder, vector<ImageFeatures> &features, vector<Size> &fullFrameSizes, Mat &fullFrame, Mat &frame, int &i)
{
	fullFrameSizes[i] = fullFrame.size();
	if(workMegapix < 0)
	{
		frame = fullFrame;
		workScale = 1;
		isWorkScaleSet = true;
	}
	else
	{
		if(!isWorkScaleSet)
		{
			workScale = min(1.0, sqrt(workMegapix * 1e6 / fullFrame.size().area()));
			isWorkScaleSet = true;
		}
		resize(fullFrame, frame, Size(), workScale, workScale);
	}
	if(!isSeamScaleSet)
	{
		seamScale = min(1.0, sqrt(seamMegapix * 1e6 / fullFrame.size().area()));
		seamWorkAspect = seamScale / workScale;
		isSeamScaleSet = true;
	}
	(*finder)(frame, features[i]);
	features[i].img_idx = i;
	LOGLN("Features in frame #" << i+1 << ": " << features[i].keypoints.size());
	resize(fullFrame, frame, Size(), seamScale, seamScale);
}
*/

/*
void displayPanoVideo(vector<concurrent_queue<Mat>*> &frameQueue, int &numVideos)
{

}
*/

//release all camera capture resources(s)
void stopMultiCapture(vector<VideoCapture*> &camCap, int &numVideos)
{
	VideoCapture *capture;
	for(int i = 0; i < numVideos; ++i)
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
void displayMultiCams(vector<concurrent_queue<Mat>*> &frameQueue, int &numVideos)
{
		//loop while key not pressed
	while(waitKey(20) != 27)
	{
		//Retrieve frames from each video capture thread
		for(int i = 0; i < numVideos; ++i)
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

//display features in frames per video
void displayFeaturesPerCam(vector<concurrent_queue<ImageFeatures>*> &featuresQueue, vector<concurrent_queue<Mat>*> &frameQueue, int &numVideos)
{
		//loop while key not pressed
	while(waitKey(20) != 27)
	{
		//Retrieve frames from each video capture thread
		for(int i = 0; i < numVideos; ++i)
		{
			Mat frame, frameKeyPoints;
			ImageFeatures features;
			//Pop frame from queue and check if frame is valid
			if(frameQueue.at(i)->try_pop(frame) && featuresQueue.at(i)->try_pop(features))
			{
				drawKeypoints(frame, features.keypoints, frameKeyPoints, Scalar(255, 0, 0), DrawMatchesFlags::DEFAULT); 
				//Show frame
				imshow(vidNames.at(i), frameKeyPoints);
			}
		}	
	}
}
