# USAGE
# python realtime_stitching.py
# import the necessary packages

from __future__ import print_function
from pyimagesearch.basicmotiondetector import BasicMotionDetector
from pyimagesearch.panorama import Stitcher
from imutils.video import VideoStream
import numpy as np
import datetime
import imutils
import time
import cv2

# initialize the video streams and allow them to warmup
print("[INFO] starting cameras...")
leftStream = VideoStream(src=1).start() #2
rightStream = VideoStream(src=0).start() #3
newStream = VideoStream(src=2).start() #1 front
new2Stream = VideoStream(src=3).start() #0 back
time.sleep(40.0)
# initialize the image stitcher, motion detector, and total
# number of frames read
stitcher = Stitcher()
motion = BasicMotionDetector(minArea=500)
total = 0

# loop over frames from the video streams
while True:
	# grab the frames from their respective video streams
	left = leftStream.read()
	right = rightStream.read()
	new = newStream.read()
	new2 = new2Stream.read()
	# resize the frames
	left = imutils.resize(left, width=400)
	right = imutils.resize(right, width=400)
	new = imutils.resize (new, width = 400)
	new2 = imutils.resize (new2, width = 400)

	# stitch the frames together to form the panorama
	# IMPORTANT: you might have to change this line of code
	# depending on how your cameras are oriented; frames
	# should be supplied in left-to-right order
	result = stitcher.stitch([new,left])
	#result = imutils.resize (result, width = 400)
	result2 = stitcher.stitch([new2,right])
	###result3 = stitcher.stitch([result, new2])
	#result3 = imutils.resize (result2, width = 400)
	#result3 = stitcher.stitch([new2, result2])
	#result4 = stitcher.stitch([left, result3])
	#result3 = imutils.resize (result2, width = 1200)
	#result4 = stitcher.stitch([right,result3])
	#result2 = imutils.resize (result2, width = 800)
	#result2 = imutils.resize (result2, width = 400)
	#result_4cam = stitcher.stitch(result, result2)
	#result3 = stitcher.stitch([result2, new2])
	#result3 = imutils.resize (result3, width = 800)
	#result_4cam = stitcher.stitch(new2, result3)
	# no homograpy could be computed
	if result is None:
		print("[INFO] homography could not be computed")
		break
	# convert the panorama to grayscale, blur it slightly, update
	# the motion detector
	gray = cv2.cvtColor(result, cv2.COLOR_BGR2GRAY)
	gray = cv2.GaussianBlur(gray, (21, 21), 0)
	locs = motion.update(gray)
	
	# only process the panorama for motion if a nice average has
	# been built up
	if total > 32 and len(locs) > 0:
		# initialize the minimum and maximum (x, y)-coordinates,
		# respectively
		(minX, minY) = (np.inf, np.inf)
		(maxX, maxY) = (-np.inf, -np.inf)

		# loop over the locations of motion and accumulate the
		# minimum and maximum locations of the bounding boxes
		for l in locs:
			(x, y, w, h) = cv2.boundingRect(l)
			(minX, maxX) = (min(minX, x), max(maxX, x + w))
			(minY, maxY) = (min(minY, y), max(maxY, y + h))

		# draw the bounding box
		cv2.rectangle(result, (minX, minY), (maxX, maxY),
			(0, 0, 255), 3)
		#cv2.rectangle(result2, (minX, minY), (maxX, maxY),
			#(0, 0, 255), 3)
	# increment the total number of frames read and draw the 
	# timestamp on the image
	total += 1
	timestamp = datetime.datetime.now()
	ts = timestamp.strftime("%A %d %B %Y %I:%M:%S%p")
	cv2.putText(result, ts, (10, result.shape[0] - 10),
		cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 0, 255), 1)
	#cv2.putText(result2, ts, (10, result.shape[0] - 10),
		#cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 0, 255), 1)
	# show the output images
	cv2.imshow("result1",result)
	cv2.imshow("Result2", result2)
	#cv2.imshow("Result3", result3)
	cv2.imshow("new2 Frame", new2)
	cv2.imshow("New Frame", new)
	cv2.imshow("Left Frame", left)
	cv2.imshow("Right Frame", right)
	key = cv2.waitKey(1) & 0xFF

	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break

# do a bit of cleanup
print("[INFO] cleaning up...")
cv2.destroyAllWindows()
leftStream.stop()
rightStream.stop()
newStream.stop()
new2Stream.stop()
