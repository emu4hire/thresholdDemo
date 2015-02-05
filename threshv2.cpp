#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int main(int argc, char ** argv){
	
	VideoCapture cam;
	cam.open(0);

	if(!cam.isOpened()){
		cout<<"Cannot get webcam input."<<endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE);

	int value = 0;
	int max =255;

	cvCreateTrackbar("Threshold Value", "Control", &value, 255);
	cvCreateTrackbar("Max Value", "Control", &max, 255);

	while(true){
		Mat frame;

		bool good=cam.read(frame);

		if(!good){
			cout<<"Cannot read frame"<<endl;
			break;
		}

		Mat frameHSV;
		double Tvalue= (double) value;
		double Tmax= (double) max;
		
		cvtColor(frame, frameHSV, CV_RGB2GRAY);
		Mat thresh;

		threshold(frameHSV, thresh, Tvalue, Tmax, 0);

		dilate( thresh, thresh, getStructuringElement(MORPH_RECT, Size(3,3)));

		imshow("Threshholded", thresh);
		imshow("Original", frame);

		if(waitKey(30) == 27)
			break;
	}

	return 0;

}
