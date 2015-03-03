#include<iostream>
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include <fstream>

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

//capture variables;
bool capture = false;
int captureNum= 0; 
ofstream outCenter; 
ofstream outMoment; 
ofstream outMouse;

void onMove(int, void *);
void onClickOriginal(int, int, int, int, void *);
void saveFrame(Mat, Mat, int);


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

	//Variables for holding the trackbar values.  We need 6, whether we're using HSV or RGB, though the max values for the hiA variable are different.
	int lowA =0;
	int hiA;
	if(colorMode == 0) 
		hiA = 179;
	else if(colorMode ==1)
		hiA = 255;
	int lowB= 0;
	int hiB = 255;
	int lowC = 0;
	int hiC = 255;

	//Initial values for the threshold ranges.
	lowerBound = Scalar(lowA, lowB, lowC);
	upperBound = Scalar(hiA, hiB, hiC);

	//Create relevent trackbars based on colorMode. 
	if(colorMode == 0){
		createTrackbar("Low H", "Control", &lowA, 179, onMove, (void*) (0));
		createTrackbar("High H", "Control", &hiA, 179, onMove, (void*) (1));
	        createTrackbar("Low S", "Control", &lowB, 255, onMove, (void*) (2));
	        createTrackbar("High S", "Control", &hiB, 255, onMove, (void*) (3));
	        createTrackbar("Low V", "Control", &lowC, 255, onMove, (void*) (4));
	        createTrackbar("High V", "Control", &hiC, 255, onMove, (void*) (5));

	}
	else if(colorMode == 1){
		createTrackbar("Low R", "Control", &lowA, 255, onMove, (void*) (0));
                createTrackbar("High R", "Control", &hiA, 255, onMove, (void*) (1));
	        createTrackbar("Low G", "Control", &lowB, 255, onMove, (void*) (2));
	        createTrackbar("High G", "Control", &hiB, 255, onMove, (void*) (3));
       		createTrackbar("Low B", "Control", &lowC, 255, onMove, (void*) (4));
	        createTrackbar("High B", "Control", &hiC, 255, onMove, (void*) (5));

	}

	setMouseCallback("Original", onClickOriginal, NULL);

	int frameNum = 0;
	//Loop runs until you can't get another frame from the video source, or a user presses escape.
	while(true){

		bool success = cap.read(src);
		if(!success){
			cerr<<"Cannot read frame from video stream"<<endl;
			break;
		}

		//Do color conversion based on colorMOde
		Mat colorImg;		
		if(colorMode == 0)
			cvtColor(src, colorImg, COLOR_BGR2HSV);
		else if(colorMode == 1)
			colorImg=src;

		Mat thresholdedImg;

  		inRange(colorImg, lowerBound, upperBound, thresholdedImg);

	        //morphological opening
       		erode(thresholdedImg,thresholdedImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	       	dilate(thresholdedImg, thresholdedImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	        //morphological closing
        	dilate(thresholdedImg, thresholdedImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	        erode(thresholdedImg, thresholdedImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		
		//Show images. 	
		imshow("Original", src);
		imshow("Thresholded", thresholdedImg);

		//Set trackbar variables to reflect any changes that may have happened to the thresholder values.
		if(colorMode ==0){
			setTrackbarPos("Low H", "Control", lowerBound[0]);
			setTrackbarPos("High H", "Control", upperBound[0]);
			setTrackbarPos("Low S", "Control", lowerBound[1]);
			setTrackbarPos("High S", "Control", upperBound[1]);
			setTrackbarPos("Low V", "Control", lowerBound[2]);
			setTrackbarPos("High V", "Control", upperBound[2]);
		}
		else if(colorMode == 1){
			setTrackbarPos("Low R", "Control", lowerBound[0]);
			setTrackbarPos("High R", "Control", upperBound[0]);
			setTrackbarPos("Low G", "Control", lowerBound[1]);
			setTrackbarPos("High G", "Control", upperBound[1]);
			setTrackbarPos("Low B", "Control", lowerBound[2]);
			setTrackbarPos("High B", "Control", upperBound[2]);
		}

		//If user presses ESC, quit stream. 
		if(waitKey(10) ==27){
			cerr<<"Video Stream Ended"<<endl;
			break;
		}
		if(waitKey(10) == 99){
			cout<<"BEGINING DATA CAPTURE. PLEASE MOUSEOVER CENTER LINE"<<endl;
			
		}
		if(waitKey(10) == 101){
			cout<<"ENDING DATA CAPTURE.  ANALYSIS WILL BE COMPLETE AT PROGRAM END"<<endl;
		}
		
		//Set threshold values to defaults if user presses Enter
		if(waitKey(10) == 13){
			cout<<"RESETING THRESHOLDS TO DEFAULTS"<<endl;
			lowerBound = Scalar(0,0,0);
			if(colorMode ==0)
				upperBound = Scalar(179, 255, 255);
			else if(colorMode ==1)
				upperBound = Scalar(255, 255, 255);
		}

		//Save frames if user presses s.
		if(waitKey(10) ==115){
			cout<<"SAVING FRAMES "<<frameNum<<endl;
			saveFrame(src, thresholdedImg, frameNum);
			frameNum++;
		}

	}

	return 0;
}

//handler for the control trackbars.
void onMove(int val, void * userdata){
	int num	= *((int*)&userdata);	

	//Userdata holds the index that we want tos set, so just set the scalar accordingly.
	if(num ==0)
		lowerBound = Scalar(val, lowerBound[1], lowerBound[2]);
	else if(num ==1)
		upperBound = Scalar(val, upperBound[1], upperBound[2]);
        else if(num ==2)
		lowerBound = Scalar(lowerBound[0], val, lowerBound[2]);
        else if(num ==3)
		upperBound = Scalar(upperBound[0], val, upperBound[2]);
        else if(num ==4)
		lowerBound = Scalar(lowerBound[0], lowerBound[1], val);
        else if(num ==5)
		upperBound = Scalar(upperBound[0], upperBound[1], val);

}

//Handles onclick events for the "original" window.
void onClickOriginal(int event, int x, int y, int flags, void * userdata){
	//If the left mouse is clicked,get the average HSV or RGB values from the area and use them for a new set of threshold ranges.

	if(event == EVENT_LBUTTONDOWN){
		Mat COLORimg;
		if(colorMode == 0)
			cvtColor(src, COLORimg, COLOR_BGR2HSV);
		else
			COLORimg=src;
	        Mat ROI= COLORimg (Rect ((x-5), (y-5), 10, 10));

        	Scalar ROImean= mean(ROI);

		//For HSV, use ranges H(14), S(60), V(80).
		if(colorMode ==0){	
			lowerBound = Scalar(ROImean[0] - 7, ROImean[1] - 30, ROImean[2] -40);
			upperBound = Scalar(ROImean[0] +7, ROImean[1] +30, ROImean[2] +40);	
		}
		
		//For RGB, use ranges R(20), G(20), B(20). (NEEDS REVISION)
		else if(colorMode ==1){
			lowerBound = Scalar(ROImean[0] -10, ROImean[1] -10, ROImean[2] -10);
			upperBound = Scalar(ROImean[0] +10, ROImean[1] +10, ROImean[2] +10);
		}
	}
}

/*
Save the orignal and thresholded frames after generating a unique filename.
*/
void saveFrame(Mat imgOriginal, Mat imgThresholded, int frameNum){
        String path= "./data/images/";
        String ext= ".png";

        String number;
        ostringstream convert;
        convert << frameNum;
        number= convert.str();

        String original= path+"original";
        String threshed= path+"thresholed";
        original=original+number;
        threshed=threshed+number;
        original=original+ext;
        threshed=threshed+ext;

        vector<int> params;
        params.push_back(CV_IMWRITE_PNG_COMPRESSION);
        params.push_back(9);


        if(!imwrite(original, imgOriginal, params))
                cerr<<"Failed to write original"<<endl;

        if(!imwrite(threshed, imgThresholded, params))
                cerr<<"Failed to write thresholded"<<endl;
}

