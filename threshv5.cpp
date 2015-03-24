#include<iostream>
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/core.hpp"
#include <cmath>
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

int mouseX;
int mouseY;
bool thresholdSet = false;
bool squareCenter = false;

void onMove(int, void *);
void onClickOriginal(int, int, int, int, void *);
void saveFrame(Mat, Mat, int);
void centerMoment(Mat &, int &, int &);
void centerFind(Mat &, int &, int &);
void lineFind(Mat &, Mat &);
void analyze(int);


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
	
	if(inputMode == 0){
		namedWindow("Thresholded", CV_WINDOW_AUTOSIZE);
		namedWindow("Original", CV_WINDOW_AUTOSIZE);
		moveWindow("Control", 500, 600);
		moveWindow("Thresholded", 60, 50);
		moveWindow("Original", 700, 50);
	}
	else if(inputMode == 1){
		namedWindow("Thresholded", CV_WINDOW_KEEPRATIO);
		namedWindow("Original", CV_WINDOW_KEEPRATIO);
		moveWindow("Control", 500, 600);
		moveWindow("Thresholded", 60, 50);
		moveWindow("Original", 500, 50);
	}

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
	int blobX=-1;
	int blobY=-1;
	//Loop runs until you can't get another frame from the video source, or a user presses escape.
	
	//capture variables

	bool capture = false;
	int captureNum= 0;
	ofstream outCenter;
	ofstream outMoment;
	ofstream outMouse;
	
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
		
		//if(squareCenter && (frameNum % 60 ==0))
		//	centerFind(thresholdedImg, blobX, blobY);
		//centerMoment(thresholdedImg,blobX, blobY);
		if(squareCenter){
			lineFind(thresholdedImg, src);
		}
	
	
			
		if(squareCenter&&(blobX +25 < src.cols && blobY+25 < src.rows && blobX -25 > 0 && blobY -25 >0)){
			Mat roi = src(Rect(Point(blobX -25, blobY-25), Point(blobX+25, blobY+25)));
			Mat color(roi.size(), CV_8UC3, Scalar(0, 255, 43)); 
	    		double alpha = 0.3;
			addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi); 
		}

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
	
		int c = waitKey(10);
		//If user presses ESC, quit stream. 
		if(c ==27){
			cerr<<"Video Stream Ended"<<endl;
			break;
		}
		//Begin data capture if user presses C.
		else if(c == 'c'){
			cout<<"BEGINING DATA CAPTURE. PLEASE MOUSEOVER CENTER LINE"<<endl;
			capture = true;

			outCenter.open("./data/captures/center.dat");
			outMoment.open("./data/captures/moment.dat");
			outMouse.open("./data/captures/mouse.dat");
		}
		//End data capture if user presses E.
		else if(c == 'e'){
			cout<<"ENDING DATA CAPTURE.  ANALYSIS WILL BE COMPLETE AT PROGRAM END"<<endl;
			capture = false;
			
			outCenter.close();
			outMoment.close();
			outMouse.close();
		}
		
		//Set threshold values to defaults if user presses Enter
		else if(c == 13){
			cout<<"RESETING THRESHOLDS TO DEFAULTS"<<endl;
			lowerBound = Scalar(0,0,0);
			if(colorMode ==0)
				upperBound = Scalar(179, 255, 255);
			else if(colorMode ==1)
				upperBound = Scalar(255, 255, 255);
			thresholdSet =false;
			squareCenter = false;
		}
		//Save frames if user presses s.
		else if(c == 's'){
			cout<<"SAVING FRAMES "<<frameNum<<endl;
			saveFrame(src, thresholdedImg, frameNum);
			frameNum++;
		}
		else if(c == 'm'){
			squareCenter =true;
		}
		
		if(capture) {
			outMoment<<captureNum<<" "<<blobX<<" "<<blobY<<endl;
			outMouse<<captureNum<<" "<<mouseX<<" "<<mouseY<<endl;
			captureNum++;
		}
	}
	analyze(captureNum);
	return 0;
}

void analyze(int captureNum){
        //Preform analysis on captures
        ifstream inMoment;
        ifstream inMouse;

        inMoment.open("./data/captures/moment.dat");
        inMouse.open("./data/captures/mouse.dat");

        int in=0;
        int momentX [captureNum];
        int momentY [captureNum];

        while(!(inMoment.eof())){
                inMoment >>in;
                inMoment >> momentX[in];
                inMoment >>momentY[in];

        }

        in =0;
        int capMouseX [captureNum];
        int capMouseY[captureNum];

        while(!(inMouse.eof())){
                inMouse >>in;
                inMouse >>capMouseX[in];
                inMouse >>capMouseY[in];
        }

        int diffX [captureNum];
        int diffY [captureNum];

        for(int i=0; i<captureNum; i++){
                diffX[i]= abs(momentX[i] - capMouseX[i]);
                diffY[i]= abs(momentY[i] - capMouseY[i]);
        }


        int meanX=0;
        int meanY=0;

        for(int j=0; j<captureNum; j++){
                meanX += diffX[j];
                meanY += diffY[j];
        }

        meanX= meanX /captureNum;
        meanY= meanY /captureNum;

        cout<<"AVERAGE X DIFFERENCE= "<<meanX<<endl;
        cout<<"AVERAGE Y DIFFERENCE= "<<meanY<<endl;

        inMoment.close();
        inMouse.close();

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
	mouseX = x;
	mouseY = y;
	if(event == EVENT_LBUTTONDOWN){
		Mat COLORimg;
		if(colorMode == 0)
			cvtColor(src, COLORimg, COLOR_BGR2HSV);
		else
			COLORimg=src;
	        Mat ROI= COLORimg (Rect ((x-5), (y-5), 10, 10));

        	Scalar ROImean= mean(ROI);

		//For HSV, use ranges H(14), S(60), V(FULL).
		if(colorMode ==0){	
			lowerBound = Scalar(ROImean[0] - 7, ROImean[1] - 30, 0);
			upperBound = Scalar(ROImean[0] +7, ROImean[1] +30, 255);	
		}
		
		//For RGB, use ranges R(20), G(20), B(20). (NEEDS REVISION)
		else if(colorMode ==1){
			lowerBound = Scalar(ROImean[0] -10, ROImean[1] -10, ROImean[2] -10);
			upperBound = Scalar(ROImean[0] +10, ROImean[1] +10, ROImean[2] +10);
		}
	
		thresholdSet=true;
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

void centerMoment(Mat & img, int & x, int & y){
	Moments oMoments=moments(img);

        double dM01= oMoments.m01;
        double dM10= oMoments.m10;
        double area= oMoments.m00;
        if(area > 10000){
  	      x =dM10/area;
              y= dM01/area;
	}

}

void centerFind(Mat & img, int & x, int & y){
	int avgX=0;
	int avgY=0;
	int counter=0;
	Scalar pixel;

	for(int i=0; i<img.cols; i++){
		for(int j=0; j<img.rows; j++){
			pixel= img.at<Scalar>(Point(i, j));
			if(pixel[2] != 0){
				counter++;
				cout<<counter<<" "<<i<<" "<<j<<endl;
			}
		}
	}
}

void lineFind(Mat & img, Mat& src){
	Scalar pixel;
	std::vector<Point> points;

	for(int i=0; i<img.cols; i++){
		for(int j=0; j<img.rows; j++){
			pixel = img.at<Scalar>(Point(i, j));
			if(pixel[2] !=0){
				points.push_back(Point(i, j));
			}
		}
	}

	Vec4f outVector;
	fitLine(points, outVector,CV_DIST_L2, 0, 0.01, 0.01);

	double m = max(img.rows, img.cols);

	Point start; 
	start.x = outVector[2] - m*outVector[0];
	start.y = outVector[3] - m*outVector[1];
	
	Point end;
	end.x = outVector[2] + m*outVector[0];
	end.y = outVector[3] + m*outVector[1];

	circle(src, Point(outVector[2], outVector[3]), 10, Scalar(255, 0,0), 1, 8, 0);	
	line(src, start, end,Scalar(0, 0, 255) ,1, 8, 0);
}
