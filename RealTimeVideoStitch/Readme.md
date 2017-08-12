# Real-time Image Stitching

## Introduction

What we are trying to do: Access multiple camera streams at once; apply image stitching and panorama construction to the frames from these video streams.
Perform motion detection in the panorama image, real-time feature points detection, and video stitching.

## Prerequisites

Installed Ubuntu 14.04 or later version or Virtual Machine
Downloaded and Installed Latest Version of OpenCV
If you don't have OpenCV already set up, download our install_opencv.sh script
Run the script sh -x install_opencv.sh
Downloaded our Makefile to build OpenCV c++ source code into executable programs
Makefile

## Program List:

* <realtime_stitching.py>
* <VideoStreaming.py>
* <motion_detector.py>
* <picamera_fps_demo.py>	
* <videostream_demo (SIFT).py>	
* <videostream_demo.py>
* <FeaturePoint_SIFT.cpp>
* <CMakeLists.txt>

## Project Structure:

* |--- Realtime_video_stitching
* |    |---- __init__.py
* |    |--- basicmotiondetector.py
* |    |--- panorama.py
* |--- realtime_stitching.py
* |--- FeaturePoint_SIFT.cpp

## Instrctions:

Note: You should have OpenCV installed on your computer and have downloaded the Makefile. Makefile should be placed into the same folder where your OpenCV source code files are located.
1. Download the whole package and extract it

2. Wherever your package is at after extraction, make a new folder "build" and place it into the same location as the "FeaturePoint_SIFT.cpp" program.

3. Let's compile the program using Makefile. Type/paste the command below into your terminal. 

- Real-time feature points detection by SIFT (webcam)
- Feature points detection from file (video, image)

Cmake required -
```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./sift
```
4. For other programs written in Python, simply run them by $ python filename.py

5. Briefly analyze the menu options below, then we will walkthrough how to execute the program from the command line.

TO RUN THE REALTIME STITCHING ALGORITHM, EXECUTE THE PROGRAM: realtime_stitching.py as such, "$python realtime_stitching.py"

The number of cameras stitched or direction/configuration of cameras can be adjusted within the program

ENTER file - Detect draw feature points from local picture file      
ENTER video - Detect draw feature points from a local video file               
ENTER live - Detect draw feature points from live streaming                   
****************
```
$ file // will load the image file and display feature points as small red circles
$ video // will load pre-recorded video to the program and display feature points
$ live // will access to the built-in/external webcams to display feature points in real-time.
```

References
●	Embedding VR view  |  Google VR  |  Google Developers. (2017, February 14). Retrieved July 28, 2017, from https://developers.google.com/vr/concepts/vrview
●	F. (2017, May 25). Facebook/Surround360. Retrieved June 10, 2017, from https://github.com/facebook/Surround360
●	Feature Detection and Description¶. (n.d.). Retrieved August 10, 2017, from http://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_table_of_contents_feature2d/py_table_of_contents_feature2d.html
●	Hatfield Sam, Yeager Luke, Young Heather. “SticHD”. Department of Computer Science & Engineering. Texas A&M University. May 8, 2012: 1-44. Web. 16 July 2017.
●	"Image Registration." Image Registration - MATLAB & Simulink. N.p., n.d. Web. 27 Apr. 2017.
●	Introduction to SIFT (Scale-Invariant Feature Transform)¶. (n.d.). Retrieved August 10, 2017, from http://docs.opencv.org/3.0-beta/doc/py_tutorials/py_feature2d/py_sift_intro/py_sift_intro.html#sift-intro
●	Julier, Simon, Yohan Baillot, Marco Lanzagorta, Dennis Brown, and Lawrence Rosenblum. 
"BARS: Battlefield Augmented Reality System." NATO Symposium on Information 
Processing Techniques for Military Systems (2000): 1-3. CiteSeer. Web. 23 Feb. 2017.
●	Klopfer, Eric, and Kurt Squire. "Environmental Detectives—the development of an augmented 
reality platform for environmental simulations." Educational Technology Research and 
Development 56.2 (2008): 203-228.
●	L. (2012, May 09). Lukeyeager/StitcHD. Retrieved July 15, 2017, from https://github.com/lukeyeager/StitcHD
●	O. (2017, August 04). Opencv/opencv. Retrieved April 5, 2017, from https://github.com/opencv/opencv
●	OpenCV panorama stitching. (2016, January 08). Retrieved August 10, 2017, from http://www.pyimagesearch.com/2016/01/11/opencv-panorama-stitching/
●	P. (2017, January 05). Multi-camera Capture using OpenCV (Multi-threaded). Retrieved July 20, 2017, from https://putuyuwono.wordpress.com/2015/05/29/multi-thread-multi-camera-capture-using-opencv/
●	Ppwwyyxx. "Ppwwyyxx/OpenPano." GitHub. N.p., 04 Aug. 2017. Web. 10 Aug. 2017.
●	Real-time panorama and image stitching with OpenCV. (2016, January 22). Retrieved August 10, 2017, from http://www.pyimagesearch.com/2016/01/25/real-time-panorama-and-image-stitching-with-opencv/
●	Sorrel Charlie. “Immersive Media’s Street View Car and 100 Megapixel Camera.” Wired. 01 June 2009. Web 16 July 2017.
●	Stitching. Images stitching¶. (n.d.). Retrieved August 10, 2017, from http://docs.opencv.org/2.4/modules/stitching/doc/stitching.html
