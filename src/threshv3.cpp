#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

Mat operation(Mat, Scalar, Scalar);
void saveframe(Mat, Mat, int);

int main( int argc, char** argv ){

	VideoCapture cap;

	if(argc >1)
		cap.open(argv[1]); //read in video from command line arguments
	else
		cap.open(0); //capture the video from web cam


	if ( !cap.isOpened() )  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE); 
		

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0; 
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);
	
	int frameNum= 0;

	while (true)
	{
		Mat imgOriginal;
		Scalar lower=  Scalar(iLowH, iLowS, iLowV);
		Scalar upper=  Scalar(iHighH, iHighS, iHighV);
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
	        }

		//Preform thrsholding operation
		Mat imgThresholded= operation(imgOriginal, lower, upper);

		imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "Video Steam Ended" << endl;
			break; 
		}

		if( waitKey(30)== 13){ //Wait for enter key press for 30ms.  If 'enter' is pressed, save the original and thresholded frames
			saveframe(imgOriginal, imgThresholded, frameNum);
		}

		frameNum++;
	}

	return 0;

}

/*
Perform thresholding operations
*/
Mat operation(Mat imgOriginal, Scalar lower, Scalar upper){

	//Convert image from BGR to HSV.
	Mat imgHSV;
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert from BGR to HSV

	//Threshold HSV images
        Mat imgThresholded;
        inRange(imgHSV, lower, upper, imgThresholded);

        //morphological opening
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        //morphological closing
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	return imgThresholded;
}

/*
Save the orignal and thresholded frames after generating a unique filename.
*/
void saveframe(Mat imgOriginal, Mat imgThresholded, int frameNum){

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
