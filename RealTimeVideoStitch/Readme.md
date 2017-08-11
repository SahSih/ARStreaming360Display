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

<VideoStreaming.py>
<motion_detector.py>
<picamera_fps_demo.py>	
<videostream_demo (SIFT).py>	
<videostream_demo.py>
<FeaturePoint_SIFT.cpp>
<CMakeLists.txt>

## Project Structure:

|--- Realtime_video_stitching
|    |---- __init__.py
|    |--- basicmotiondetector.py
|    |--- panorama.py
|--- realtime_stitching.py
|--- FeaturePoint_SIFT.cpp

## Instrctions:

Note: You should have OpenCV installed on your computer and have downloaded the Makefile. Makefile should be placed into the same folder where your OpenCV source code files are located.
1. Download the whole package and extract it

2. Wherever your package is at after extraction, make a new folder "build" and place it into the same location as the "FeaturePoint_SIFT.cpp" program.

3. Let's compile the program using Makefile. Type/paste the command below into your terminal. 

- Real-time feature points detection by SIFT (webcam)
- Feature points detection from file (video, image)

Cmake required -

$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./sift

4. For other programs written in Python, simply run them by $ python filename.py

5. Briefly analyze the menu options below, then we will walkthrough how to execute the program from the command line.

ENTER file - Detect draw feature points from local picture file      
ENTER video - Detect draw feature points from a local video file               
ENTER live - Detect draw feature points from live streaming                   
****************

$ file // will load the image file and display feature points as small red circles
$ video // will load pre-recorded video to the program and display feature points
$ live // will access to the built-in/external webcams to display feature points in real-time.

