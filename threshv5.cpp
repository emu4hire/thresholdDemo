#include<iostream>
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

//Input Mat, and Scalar bounds for thresholding.
Mat src;
Scalar upperBound;
Scalar lowerBound;

//Mode variables. Defaulted at -1 for checking.
int colorMode=-1; //0 for HSV, 1 for RGB.
int inputMode=-1; //0 FOr webcam, 1 for video file.

//File path variables
int videoFilePath;


Mat threshold(int);
void setThreshold(Scalar, Scalar);

int main(int argc, char ** argv){

	//Input parsing to select color mode, input mode and anything else needed.	
	if(argc > 1){
		int num=1;

		while(num < argc){

			string input= argv[num];
			
			//Help text.
			if(input == "-help" || input == "-h" || input == "-H"){
		                cout<<"Available options for v5:"<<endl;
		                cout<<"-w       Use a webcam input"<<endl;
		                cout<<"-f       Use a video file (Follow with a valid file path)"<<endl;
        		        cout<<"-HSV     Use HSV images and values to preform thresholding"<<endl;
		                cout<<"-RGB     Use RGB images and values to preofrm thresholding"<<endl;
	
			}
			else if(input == "-w" || input == "-W"){
				inputMode=0;
			}
			else if(input == "-v" || input == "-V"){
				if((num+1) <argc){
					inputMode=1;
					videoFilePath=num+1;
					num++;
				}
				else
					cout<<"Please follow -v with a path to a video file.";
			}
			else if(input== "-HSV" || input == "-hsv"){
				colorMode=0;
			}
			else if(input== "-RGB" || input == "-rgb"){
				colorMode=1;
			}
			else{
				cout<<"Unrecognized option"<<endl;
			}

			num++;
		}

	}
	//If there is no input, simply use defaults: webcam input and HSV color mode.
	else{
		colorMode= 0;
		inputMode= 0;
	}


	if(colorMode ==-1)
		colorMode=0;
	if(inputMode == -1)
		inputMode = 0;

	
	//Input completed, now we move to the real stuff.

	//Open the selected input.	
	VideoCapture cap;

	if(inputMode ==0)
		cap.open(0);
	else if(inputMode ==1)
		cap.open(argv[videoFilePath]);
	
	//If some error happens with video input, show an error message.
	if(!cap.isOpened()){
		cerr<< "Was unable to open video input."<<endl;
		if(inputMode ==1)
			cerr<<"Filepath inputed ="<<argv[videoFilePath]<<endl<<"Please input a valid filepath."<<endl;
		else
			cerr<<"Please check your input method."<<endl;

		return -1;
	}
	
	//Create our three windows for control of the threshold, display of the original image, and the display of the thresholded image.

	namedWindow("Control", CV_WINDOW_AUTOSIZE);
	namedWindow("Thresholded", CV_WINDOW_AUTOSIZE);
	namedWindow("Original", CV_WINDOW_AUTOSIZE);

	if(colorMode == 0){
		int lowA=0;
		int lowB=0;
		int lowC=0;
		
		int highA;
		if(colorMode == 0)
			highA=179;
		else
			highA=255;

		int highB=255;
		int highC=255;
		
                cvCreateTrackbar("Low H/R", "Control", &lowA, 255); 
                cvCreateTrackbar("High H/R", "Control", &highA, 255);

                cvCreateTrackbar("Low S/G", "Control", &lowB, 255); 
                cvCreateTrackbar("High S/G", "Control", &highB, 255);

                cvCreateTrackbar("Low V/B", "Control", &lowC, 255); 
                cvCreateTrackbar("High V/B", "Control", &highC, 255);

		
	}


	while(1){
		if(cap.read(src)){
			cerr<<"Cannot read frame from video stream"<<endl;
			break;
		}

		Mat thresholdedImg= threshold(colorMode);
		
		imshow("Original", src);
		imshow("Thresholded",thresholdedImg);

		if(waitKey(30) ==27){
			cerr<<"Video Stream Ended"<<endl;
			break;
		}

	}


	return 0;


}

Mat threshold(int mode){

	Mat colorImg;

	if(mode == 0){
		cvtColor(src, colorImg, COLOR_BGR2HSV);
	}
	else
		colorImg=src;

	Mat returnImg;
	inRange(colorImg, lowerBound, upperBound, returnImg);

	//morphological opening
        erode(returnImg, returnImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate(returnImg, returnImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        //morphological closing
        dilate(returnImg, returnImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode(returnImg, returnImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	return returnImg;
}
