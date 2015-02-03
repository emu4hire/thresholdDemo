#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int argc, char* argv[]){

	double thresh_value= 0;
	double max_value = 0;
	int thresh_type =0;
	
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
	cam.open(0);
	
	if(!cam.isOpened())
		return -1;

	Mat frame, gray, edited;


	namedWindow("Thresholding Demo", 1);

	for(;;){
		cam >> frame;
		if(thresh_type !=5){
			cvtColor(frame, gray, CV_RGB2GRAY);
			threshold(gray, edited, thresh_value, max_value, thresh_type);
			imshow("Thresholding Demo", edited);
		}
		else
			imshow("Thresholding Demo", frame);
		if(waitKey(30)>=0) break;


	}
	return 0;
}

