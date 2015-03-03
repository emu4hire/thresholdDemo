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


Mat thresholdOperation(int);
void onMove(int, void *);

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

	int lowA =0;
	int hiA;
	if(colorMode == 0) 
		hiA = 179;
	else
		hiA = 255;
	int lowB= 0;
	int hiB = 255;
	int lowC = 0;
	int hiC = 255;

	lowerBound = Scalar(lowA, lowB, lowC);
	upperBound = Scalar(hiA, hiB, hiC);

	if(colorMode == 0){
		createTrackbar("Low H", "Control", &lowA, 179, onMove, (void*) (0));
		createTrackbar("High H", "Control", &hiA, 179, onMove, (void*) (1));
	        createTrackbar("Low S", "Control", &lowB, 255, onMove, (void*) (2));
	        createTrackbar("High S", "Control", &hiB, 255, onMove, (void*) (3));
	        createTrackbar("Low V", "Control", &lowC, 255, onMove, (void*) (4));
	        createTrackbar("High V", "Control", &hiC, 255, onMove, (void*) (5));

	}
	else{
		createTrackbar("Low R", "Control", &lowA, 255, onMove, (void*) (0));
                createTrackbar("High R", "Control", &hiA, 255, onMove, (void*) (1));
	        createTrackbar("Low G", "Control", &lowB, 255, onMove, (void*) (2));
	        createTrackbar("High G", "Control", &hiB, 255, onMove, (void*) (3));
       		createTrackbar("Low B", "Control", &lowC, 255, onMove, (void*) (4));
	        createTrackbar("High B", "Control", &hiC, 255, onMove, (void*) (5));

	}

	while(true){
		bool success = cap.read(src);
		if(!success){
			cerr<<"Cannot read frame from video stream"<<endl;
			break;
		}

		Mat colorImg;		

		if(colorMode == 0)
			cvtColor(src, colorImg, COLOR_BGR2HSV);
		else
			colorImg=src;

		Mat thresholdedImg;
		thresholdedImg = thresholdOperation(colorMode);
		
		imshow("Original", src);
		imshow("Thresholded", thresholdedImg);

		if(waitKey(30) ==27){
			cerr<<"Video Stream Ended"<<endl;
			break;
		}

	}


	return 0;


}

Mat thresholdOperation(int mode){

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

void onMove(int val, void * userdata){
	int num	= *((int*)&userdata);	

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
