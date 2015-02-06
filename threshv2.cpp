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

		Mat threshGRAY;
		double Tvalue= (double) value;
		double Tmax= (double) max;
		
		Mat thresh;

		threshold(frame, thresh, Tvalue, Tmax, 1);
		cvtColor(thresh, threshGRAY, CV_RGB2GRAY);
		dilate( threshGRAY, threshGRAY, getStructuringElement(MORPH_RECT, Size(3,3)));

		imshow("Threshholded", threshGRAY);
		imshow("Original", frame);

		if(waitKey(30) == 27)
			break;
	}

	return 0;

}
