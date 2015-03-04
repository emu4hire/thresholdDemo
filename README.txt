THRESHOLD DEMO (Michael Fulton) Spring 2015
=================================================================
This is a tool designed for use in defining thresholding algorithms for object tracking based on color.

There are currently 5 versions, with the fifth considered a stable release.  

V1 allows the use of the built-in threshold function on webcam or video.  Explination is inherant in use.
V2 uses builtin binary threshold on webcam input (Use not recommended). 
V3 uses a range-based threshold with a control window to threshold images from webcam.
V4 uses the same algorithm, but allows video input.

V5 thresholds based on a range of color values on either webcam or video input, using either HSV or RGB color values.
	Command line use:
		-w	For webcam input
		-v	For video input (follow imediately with filepath to video)
		-hsv	For HSV color mode
		-rgb	For RGB color mode

If no command line arguments are given, V5 defaults to HSV and webcam input.

Once used, it displays the original image, the thresholded image, and a control window. 
	Use:
		Click on an area in the Original window to get the average HSV or RGB values of that area used as a basis for a thresholding range.
		Press enter to set threshold ranges to default (no thresholding) and to cancel square center detection (M).
		Press ESC to end the program.
		Press M to use center detection to put a square around the detected center.
		Press C to enter capture mode, saving detected center values and coordinates of the mouse on the original window in files in ./data/captures/
		Press E to end capture.  Further captures will overwrite this capture.  Analysis is run at program's termination.
		Press S to save the original and thresholded frames to ./data/images. 

	
