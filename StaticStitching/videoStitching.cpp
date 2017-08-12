//OpenFrames
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <ctime>
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

time_t timev = time(nullptr);

/*Help Message that shows how to use CLI to interact with program*/
static void printProgUsage();

/*Default CLI Args*/

//this holds video files
vector<String> vidNames;
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

//main camera capturing process that'll be done by thread(s)
void captureFrame(vector<VideoCapture*> &camCap, vector<concurrent_queue<ImageFeatures>*> &featuresQueue, vector<concurrent_queue<Mat>*> &frmQueue, Ptr<FeaturesFinder> &finder, vector<Size> &fullFrameSizes, int numVideos);

/*
//detect feature keypoints per video frame
void detectFeaturesPerVideoFrames(Ptr<FeaturesFinder> &finder, vector<ImageFeatures> &features, vector<Size> &fullFrameSizes, Mat &fullFrame, Mat &frame, int &i);
*/

//match feature keypoints across multiple frames that come from different videos
void matchFeatures(Ptr<FeaturesMatcher> &matcher, vector<concurrent_queue<ImageFeatures>*> &featuresQueue, concurrent_queue<vector<MatchesInfo>> &pairWiseMatchesQueue, vector<concurrent_queue<Mat>*> &frameQueue, int &numVideos);

//release all camera capture resources(s)
void stopMultiCapture(vector<VideoCapture*> &camCap, int &numVideos);

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
	cout << "Starting Stitching at date,time: " << asctime(localtime(&timev)) << endl;
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


LOG("Pairwise matching");
#if ENABLE_LOG
	t = getTickCount();
#endif	

/**
*	Initialize FeaturesMatcher for finding matched features across all videos
*/

	concurrent_queue<vector<MatchesInfo>> pairWiseMatchesQueue;
	Ptr<FeaturesMatcher> matcher;

	if (matcherType == "affine")
		matcher = makePtr<AffineBestOf2NearestMatcher>(false, tryCuda, matchConf);
	else if (rangeWidth==-1)
		matcher = makePtr<BestOf2NearestMatcher>(tryCuda, matchConf);
	else
		matcher = makePtr<BestOf2NearestRangeMatcher>(rangeWidth, tryCuda, matchConf);


	//find all feature keypoint matches across multiple frames of different videos
	matchFeatures(matcher, featuresQueue, pairWiseMatchesQueue, frameQueue, numVideos);

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
			return;
		}
		//cout << "Video File Setup: " << (vidNames.at(i)) << endl;
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
	//cout << "Inside captureFrame:" << endl;
	//cout << "numVideos = " << numVideos << endl;
	VideoCapture *capture = camCap.at(numVideos);
	int i = 0;
	Mat fullFrame, frame;
	ImageFeatures features;
	while(true)
	{
		//Grab frame from camera capture, if successful
		if((*capture).read(fullFrame))
		{
			//cout << "Going to push in frame from vidIndex: " << numVideos << endl;
			//feature Detection on frame before it gets pushed to frameQueue
			//detectFeaturesPerVideoFrame(finder, features, fullFrameSizes, fullFrame, frame, i);

			fullFrameSizes[i] = fullFrame.size();
			if(fullFrame.empty())
			{
				LOGLN("Can't open frame " << vidNames.at(numVideos));
				return;
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
			//LOGLN("Features in frame #" << i+1 << ": " << features.keypoints.size());
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

//Hardcoded for 4 video files or 4 video cameras
//finds the matching features of 4 frames from 4 different vids/cameras
void matchFeatures(Ptr<FeaturesMatcher> &matcher, vector<concurrent_queue<ImageFeatures>*> &featuresQueue, concurrent_queue<vector<MatchesInfo>> &pairWiseMatchesQueue, vector<concurrent_queue<Mat>*> &frameQueue, int &numVideos)
{

	cout << "Enter MatchFeatures function" << endl;
	/*
	*	Initialize for Matching Features
	*/
	concurrent_queue<vector<String>> frameNamesQueue;
	concurrent_queue<vector<Mat>> framesQueue;
	concurrent_queue<vector<Size>> fullFrameSizesQueue;

	ImageFeatures features0, features1, features2, features3;
	vector<ImageFeatures> ftrsAcrsVidFrames = {features0, features1, features2, features3};

	Mat frame0, frame1, frame2, frame3;
	vector<Mat> frameAcrsEachVideos = {frame0, frame1, frame2, frame3};

	bool tryPopFeatures = featuresQueue.at(0)->try_pop(ftrsAcrsVidFrames[0]) && featuresQueue.at(1)->try_pop(ftrsAcrsVidFrames[1]) && featuresQueue.at(2)->try_pop(ftrsAcrsVidFrames[2]) && featuresQueue.at(3)->try_pop(ftrsAcrsVidFrames[3]);
	
	bool tryPopFrames = frameQueue[0]->try_pop(frameAcrsEachVideos[0]) && frameQueue[1]->try_pop(frameAcrsEachVideos[1]) && frameQueue[2]->try_pop(frameAcrsEachVideos[2]) && frameQueue[3]->try_pop(frameAcrsEachVideos[3]);

	/*
	*	Initialize for Estimator
	*	Estimator estimates the rotations of all cameras by taking
	*	the features of all frames and the pairwise matches between them
	*/

	Ptr<Estimator> estimator;

 	if (estimatorType == "affine")
		estimator = makePtr<AffineBasedEstimator>();
	else
		estimator = makePtr<HomographyBasedEstimator>();

	concurrent_queue<vector<CameraParams>> camerasQueue;

	/*
	*	Initialize for Adjuster 
	*	Adjuster performs camera refinement methods
	*/

	Ptr<detail::BundleAdjusterBase> adjuster;
	
	if (baCostFunc == "reproj") 
		adjuster = makePtr<detail::BundleAdjusterReproj>();
	else if (baCostFunc == "ray") 
		adjuster = makePtr<detail::BundleAdjusterRay>();
	else if (baCostFunc == "affine") 
		adjuster = makePtr<detail::BundleAdjusterAffinePartial>();
	else if (baCostFunc == "no") 
		adjuster = makePtr<NoBundleAdjuster>();
	else
	{
		cout << "Unknown bundle adjustment cost function: '" << baCostFunc << "'.\n";
		return;
	}

	adjuster->setConfThresh(confThresh);
	Mat_<uchar> refineMask = Mat::zeros(3, 3, CV_8U);
	if (baRefineMask[0] == 'x') 
		refineMask(0,0) = 1;
	if (baRefineMask[1] == 'x') 
		refineMask(0,1) = 1;
	if (baRefineMask[2] == 'x') 
		refineMask(0,2) = 1;
	if (baRefineMask[3] == 'x') 
		refineMask(1,1) = 1;
	if (baRefineMask[4] == 'x') 
		refineMask(1,2) = 1;
	adjuster->setRefinementMask(refineMask);

	/*
	*
	*	Initialize Warper Creator and RotationWarper
	*
	*/

	// Warp images and their masks

	Ptr<WarperCreator> warperCreator;
#ifdef HAVE_OPENCV_CUDAWARPING
	if (tryCuda && cuda::getCudaEnabledDeviceCount() > 0)
	{
		if (warpType == "plane")
			warperCreator = makePtr<cv::PlaneWarperGpu>();
		else if (warpType == "cylindrical")
			warperCreator = makePtr<cv::CylindricalWarperGpu>();
		else if (warpType == "spherical")
			warperCreator = makePtr<cv::SphericalWarperGpu>();
	}
	else
#endif
	{
		if (warpType == "plane")
			warperCreator = makePtr<cv::PlaneWarper>();
		else if (warpType == "affine")
			warperCreator = makePtr<cv::AffineWarper>();
		else if (warpType == "cylindrical")
			warperCreator = makePtr<cv::CylindricalWarper>();
		else if (warpType == "spherical")
			warperCreator = makePtr<cv::SphericalWarper>();
		else if (warpType == "fisheye")
			warperCreator = makePtr<cv::FisheyeWarper>();
		else if (warpType == "stereographic")
			warperCreator = makePtr<cv::StereographicWarper>();
		else if (warpType == "compressedPlaneA2B1")
			warperCreator = makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
		else if (warpType == "compressedPlaneA1.5B1")
			warperCreator = makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
		else if (warpType == "compressedPlanePortraitA2B1")
			warperCreator = makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
		else if (warpType == "compressedPlanePortraitA1.5B1")
			warperCreator = makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
		else if (warpType == "paniniA2B1")
			warperCreator = makePtr<cv::PaniniWarper>(2.0f, 1.0f);
		else if (warpType == "paniniA1.5B1")
			warperCreator = makePtr<cv::PaniniWarper>(1.5f, 1.0f);
		else if (warpType == "paniniPortraitA2B1")
			warperCreator = makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
		else if (warpType == "paniniPortraitA1.5B1")
			warperCreator = makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
		else if (warpType == "mercator")
 			warperCreator = makePtr<cv::MercatorWarper>();
		else if (warpType == "transverseMercator")
			warperCreator = makePtr<cv::TransverseMercatorWarper>();
	}

	if (!warperCreator)
	{
		cout << "Can't create the following warper '" << warpType << "'\n";
		return;
	}

    Ptr<RotationWarper> warper;

	/*
	*
	*	Initialize ExposureCompensator
	*
	*/

	Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(exposCompType);

	/*
	*
	*	Initialize SeamFinder
	*
	*/	

	Ptr<SeamFinder> seamFinder;

	if (seamFindType == "no")
		seamFinder = makePtr<detail::NoSeamFinder>();
	else if (seamFindType == "voronoi")
		seamFinder = makePtr<detail::VoronoiSeamFinder>();
	else if (seamFindType == "gc_color")
	{
#ifdef HAVE_OPENCV_CUDALEGACY
		if (tryCuda && cuda::getCudaEnabledDeviceCount() > 0)
			seamFinder = makePtr<detail::GraphCutSeamFinderGpu>(GraphCutSeamFinderBase::COST_COLOR);
		else
#endif
			seamFinder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR);
	}
	else if (seamFindType == "gc_colorgrad")
	{
#ifdef HAVE_OPENCV_CUDALEGACY
		if (tryCuda && cuda::getCudaEnabledDeviceCount() > 0)
			seamFinder = makePtr<detail::GraphCutSeamFinderGpu>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
		else
#endif
			seamFinder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
    	}
	else if (seamFindType == "dp_color")
		seamFinder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR);
	else if (seamFindType == "dp_colorgrad")
		seamFinder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR_GRAD);
	if (!seamFinder)
	{
		cout << "Can't create the following seam finder '" << seamFindType << "'\n";
		return;
	}

	/*
	*
	*	Initialize Blender and Timelapser
	*
	*/

	
	Ptr<Blender> blender;
	Ptr<Timelapser> timelapser;

	int index = 0;
	//loop while key not pressed and perform video stitching per frames
	while(waitKey(20) != 27)
	{
		//cout << "Enter While loop of MatchFeatures" << endl;
		//Pop frame from queue and check if frame is valid
		if(tryPopFeatures && tryPopFrames)
		{
			cout << "Successfully Popped Features across Frames #" << index << endl;
			vector<MatchesInfo> pairWiseMatches;
			(*matcher)(ftrsAcrsVidFrames, pairWiseMatches);

			// Check if we should save matches graph
			// saves a new matching graph for each frame set number
			if(saveGraph)
			{
				LOGLN("Saving matches graph for each frame set number...");
				ofstream f((saveGraphTo + "frameSetNum_" + to_string(index)).c_str());
				f << matchesGraphAsString(vidNames, pairWiseMatches, confThresh);
			}
			
			// Leave only frames we are sure are from the same panorama
			vector<int> indices = leaveBiggestComponent(ftrsAcrsVidFrames, pairWiseMatches, confThresh);
			vector<Mat> frameSubset;
			vector<String> frameNamesSubset;
			vector<Size> fullFrameSizesSubset;
		
			//TODO: Current Problem vidNames on next iteration will be duplicate
			//Potential Solution on next iteration differ 'vidNames' vector
			for(size_t i = 0; i < indices.size(); ++i)
			{
				frameNamesSubset.push_back(vidNames[indices[i]]);
				frameSubset.push_back(frameAcrsEachVideos[indices[i]]);
				fullFrameSizesSubset.push_back(frameAcrsEachVideos[indices[i]].size());
			}
			
			

			//Check if we still have enough frames
			int numFrames = static_cast<int>(frameNamesSubset.size());
			if(numFrames < 2)
			{
				cout << "Need more frames at iteration " << index << endl;
				return;
			}
				
			vector<CameraParams> cameras;
			bool estimateCamParams = !(*estimator)(ftrsAcrsVidFrames, pairWiseMatches, cameras);
			if(estimateCamParams)
			{
				cout << "Homography estimation failed.\n";
				return;	
			}

			for(size_t i = 0; i < cameras.size(); ++i)
			{
				Mat R;
				cameras[i].R.convertTo(R, CV_32F);
				cameras[i].R = R;
				cout << "Initial camera intrinsics #" << indices[i]+1 << ":\nK:\n" << cameras[i].K() << "\nR:\n" << cameras[i].R << endl;
			}

			//Adjuster performs camera refinement methods

			if (!(*adjuster)(ftrsAcrsVidFrames, pairWiseMatches, cameras))
			{
				cout << "Camera parameters adjusting failed.\n";
				return;
			}

			//Find median focal length
			vector<double> focals;
			for(size_t i = 0; i < cameras.size(); ++i)
			{
				cout << "Camera #" << indices[i]+1 << ":\nK:\n" << cameras[i].K() << "\nR:\n" << cameras[i].R << endl; 
				focals.push_back(cameras[i].focal);
			}

			sort(focals.begin(), focals.end());
			float warpedFrameScale;
			if(focals.size() % 2 == 1)
				warpedFrameScale = static_cast<float>(focals[focals.size() / 2]);
			else
				warpedFrameScale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2] * 0.5f);

			if(doWaveCorrect)
			{
				vector<Mat> rmats;
				for(size_t i = 0; i < cameras.size(); ++i)
					rmats.push_back(cameras[i].R.clone());
				waveCorrect(rmats, wave_correct);
				for(size_t i = 0; i < cameras.size(); ++i)
					cameras[i].R = rmats[i];
			}
			
			//Prepare frames masks
			vector<Point> corners(numFrames);
			vector<UMat> masksWarped(numFrames);
			vector<UMat> framesWarped(numFrames);
			vector<Size> sizes(numFrames);
			vector<UMat> masks(numFrames);


			for (int i = 0; i < numFrames; ++i)
			{
				masks[i].create(frameSubset[i].size(), CV_8U);
				masks[i].setTo(Scalar::all(255));
			}

			warperCreator->create(static_cast<float>(warpedFrameScale * seamWorkAspect));

			for(int i = 0; i < numFrames; ++i)
			{
				Mat_<float> K;
				cameras[i].K().convertTo(K, CV_32F);
				float swa = (float)seamWorkAspect;
				K(0,0) *= swa;
				K(0,2) *= swa;
				K(1,1) *= swa;
				K(1,2) *= swa;

				corners[i] = warper->warp(frameSubset[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, framesWarped[i]);
				sizes[i] = framesWarped[i].size();

				warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masksWarped[i]);
			}

			vector<UMat> framesWarpedF(numFrames);
			for(int i = 0; i < numFrames; ++i)
				framesWarped[i].convertTo(framesWarpedF[i], CV_32F);

			cout << "Warping frames for iteration " + to_string(index) + " done" << endl;


			//Feed Compensator function
			compensator->feed(corners, framesWarped, masksWarped);

			//SeamFinder
			seamFinder->find(framesWarpedF, corners, masksWarped);

			//Compositing
			Mat frmWarped, frmWarpedS;
			Mat dilatedMask, seamMask, mask, maskWarped;
			//double composeSeamAspect = 1;
			double composeWorkAspect = 1;
			
			VideoWriter writerPanoFile;
			//ELP 170 Deg Fisheye 30 FPS
			double fps = 30.0;

			cout << "Made it to for loop that writes stitched Pano frame to file\n";
			Mat frameResized;
			for(int frm_idx = 0; frm_idx < numFrames; ++frm_idx)
			{
				cout << "Compositing frame #" << indices[frm_idx]+1 << endl;

				// already read the frames back in cameraFrameCapture
				//frameAcrsEachVideos[frm_idx]
				if(!isComposeScaleSet)
				{
					if(composeMegapix > 0)
					{
						composeScale = min(1.0, sqrt(composeMegapix * 1e6 / frameAcrsEachVideos[frm_idx].size().area()));
					}
					isComposeScaleSet = true;

					// Compute relative scales
            				//composeSeamAspect = composeScale / seamScale;
					composeWorkAspect = composeScale / workScale;

					// Update warped frame scale
					warpedFrameScale *= static_cast<float>(composeWorkAspect);
					warper = warperCreator->create(warpedFrameScale);

					// Update corners and sizes
					for(int i = 0; i < numFrames; ++i)
					{
						// Update intrinsics
						cameras[i].focal *= composeWorkAspect;
						cameras[i].ppx *= composeWorkAspect;
						cameras[i].ppy *= composeWorkAspect;

						// Update corner and size
						Size sz = frameAcrsEachVideos[i].size();
						if(std::abs(composeScale - 1) > 1e-1)
						{
sz.width = cvRound(frameAcrsEachVideos[i].size().width * composeScale);
							sz.height = cvRound(frameAcrsEachVideos[i].size().height * composeScale);
						}

						Mat K;
						cameras[i].K().convertTo(K, CV_32F);
						Rect roi = warper->warpRoi(sz, K, cameras[i].R);
						corners[i] = roi.tl();
						sizes[i] = roi.size();	
					}
				}

				if(abs(composeScale - 1) > 1e-1)
				{
					resize(frameAcrsEachVideos[frm_idx], frameResized, Size(), composeScale, composeScale);
				}
				else
				{
					frameResized = frameAcrsEachVideos[frm_idx];
				}
				
				Size frmSize = frameResized.size();
				
				Mat K;
				cameras[frm_idx].K().convertTo(K, CV_32F);

				// Warp the current frame
				warper->warp(frameResized, K, cameras[frm_idx].R, INTER_LINEAR, BORDER_REFLECT, frmWarped);
				// Warp the current frame mask
				mask.create(frmSize, CV_8U);
				mask.setTo(Scalar::all(255));
				warper->warp(mask, K, cameras[frm_idx].R, INTER_NEAREST, BORDER_CONSTANT, maskWarped);
				
				// Compensate exposure
				compensator->apply(frm_idx, corners[frm_idx], frmWarped, maskWarped); 
				
				frmWarped.convertTo(frmWarpedS, CV_16S);

				dilate(masksWarped[frm_idx], dilatedMask, Mat());
				resize(dilatedMask, seamMask, maskWarped.size());
				maskWarped = seamMask & maskWarped;

				if(!blender && !timelapse)
				{
					blender = Blender::createDefault(blendType, tryCuda);
					Size dstSz = resultRoi(corners, sizes).size();
					float blendWidth = sqrt(static_cast<float>(dstSz.area())) * blendStrength / 100.f;
					if(blendWidth < 1.f)
					{
						blender = Blender::createDefault(Blender::NO, tryCuda);
					}
					else if(blendType == Blender::MULTI_BAND)
					{
						MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(blender.get());
						mb->setNumBands(static_cast<int>(ceil(log(blendWidth)/log(2.)) - 1.));
						cout << "Multi-band blender, number of bands: " << mb->numBands() << endl;
					}
					else if(blendType == Blender::FEATHER)
					{
						FeatherBlender* fb = dynamic_cast<FeatherBlender*>(blender.get());
						fb->setSharpness(1.f/blendWidth);
						LOGLN("Feather blender, sharpness: " << fb->sharpness());
					}
					blender->prepare(corners, sizes);
				}
				else if(!timelapser && timelapse)
				{
					timelapser = Timelapser::createDefault(timelapseType);
					timelapser->initialize(corners, sizes);
				}
					
				// Blend the current frame
				if(timelapse)
				{
					timelapser->process(frmWarpedS, Mat::ones(frmWarpedS.size(), CV_8UC1), corners[frm_idx]);
					String fixedFileName;
					size_t posS = String(vidNames[frm_idx]).find_last_of("/\\");
					if(posS == String::npos)
					{
						fixedFileName = "fixed_" + vidNames[frm_idx];
					}
					else
					{
						fixedFileName = "fixed_" + String(vidNames[frm_idx]).substr(posS + 1, String(vidNames[frm_idx]).length() - posS);
					}
					

					writerPanoFile.open(fixedFileName, CV_FOURCC('M','J','P','G'), fps, timelapser->getDst().size());
					if(!writerPanoFile.isOpened())
					{
						cout << "Failed to write to video\n";
					}
					//Convert UMat to Mat
					//https://l.facebook.com/l.php?u=https%3A%2F%2Fstackoverflow.com%2Fquestions%2F27445398%2Fhow-to-read-umat-from-a-file-in-opencv-3-0-beta%2F27530966%2327530966&h=ATMi7koVOPGdKUH5SkT2K4FFgz0yQJK8W1RPIonoWWLbIGvx4shezu7iC0AuGxxOzjXCFTs_VM6ccbHvQZfjfHWzgUS3XxGezKN3jeFaCgbz3h0h1i5OcskqysJIYDbYU45P
					Mat frameDst;
					frameDst = timelapser->getDst().getMat(cv::ACCESS_READ);
					writerPanoFile.write(frameDst);
				}// VideoWriter writes stitched frame to file
				else
				{
					blender->feed(frmWarpedS, maskWarped, corners[frm_idx]);
				}

				frameAcrsEachVideos[frm_idx].release();
				frmWarped.release();
				frameResized.release();
				mask.release();

			}//end of for loop

			if(!timelapse)
			{
				Mat result, resultMask;
				Size resultSize = frameSubset[0].size();
				blender->blend(result, resultMask);

				cout << "Compositing done" << endl;
				writerPanoFile.open(resultName, CV_FOURCC('M','J','P','G'), fps, result.size());
				writerPanoFile.write(result);
				imshow("Real-Time VideoStitching Feed", result);
			}

			pairWiseMatchesQueue.push(pairWiseMatches);
			framesQueue.push(frameSubset);
			frameNamesQueue.push(frameNamesSubset);
			fullFrameSizesQueue.push(fullFrameSizesSubset);
			camerasQueue.push(cameras);

			// Release Unused Memory
			frameSubset.clear();
			framesWarped.clear();
			framesWarpedF.clear();
			masks.clear();

			//Start Compositing
			index++;	
		} 	
	}
	(*matcher).collectGarbage();
	
	cout << "Finished at date,time: " << asctime(localtime(&timev)) << endl;
	
	
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
