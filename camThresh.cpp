#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int argc, char* argv[]){

	double thresh_value= 0;
	double max_value = 0;
	int thresh_type =0;
	int input_type;
	char filename [50];


	std::cout <<"Please input (0) for a webcam or (1) for video:";	
	std::cin>>input_type;
	if(input_type==1){
		std::cout<<"Please input filename:";
		std::cin>>filename;
	}
	
 	std::cout <<"Please input your threshold type."<<std::endl;
	std::cout <<"(0) for Binary, (1) for Binary inverted, (2) for Truncate,(3) for To Zero, (4) for To Zero Inverted, (5) for NO THRESHHOLD"<<std::endl;
	std::cin >> thresh_type;
	if(thresh_type !=5){

		std::cout << "Please input the threshold value:";
		std::cin >> thresh_value;
		std::cout << std::endl <<"Please input the maximum value:";
		std::cin >> max_value;
		std::cout << std::endl;
	}

	VideoCapture cam;
	
	if(input_type=0){
		cam.open(0);


		if(!cam.isOpened())
			return -1;
	}
	else{
		cam.open(filename);
		
	}
	Mat frame, gray, edited;


	namedWindow("Thresholding Demo", WINDOW_AUTOSIZE);

	for(;;){
		cam >> frame;
		if(thresh_type !=5){
			threshold(frame, edited, thresh_value, max_value, thresh_type);	
			cvtColor(edited, gray, CV_RGB2GRAY);
			imshow("Thresholding Demo", gray);
		}
		else
			imshow("Thresholding Demo", frame);
		if(waitKey(30)>=0) break;


	}
	return 0;
}

