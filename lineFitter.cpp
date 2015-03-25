#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char ** argv){
	VideoCapture in;
	VideoWriter out;

	if(argc >1){
		in.open(argv[1]);
//		int ex= static_cast<int>(in.get(CV_CAP_PROP_FOURCC));
		const string source = argv[1];
		string::size_type pAt = source.find_last_of('.');
		const string NAME= source.substr(0, pAt) + "E.avi";
	
		out.open(NAME, CV_FOURCC('D', 'I', 'V', 'X') , in.get(CV_CAP_PROP_FPS), Size( (int) in.get(CV_CAP_PROP_FRAME_WIDTH), (int) in.get(CV_CAP_PROP_FRAME_HEIGHT)), true);	
	}

	else
		exit(1);

	if(!in.isOpened()){
		cerr<<"Was unable to open video input."<<endl;
	}
	
	if(!out.isOpened()){
		cerr<<"Was unable to open video output."<<endl;
	}

	int count=0;

	while(true){
		Mat frame;
		Mat colorImg;
		Mat threshed;

		bool success = in.read(frame);

		if(!success){
			cout<<"VIDEO STREAM COMPLETE"<<endl;
			break;
		}
	
		//Lets do some thresholding.
	
		cvtColor(frame, colorImg, COLOR_BGR2HSV);

		inRange(colorImg, Scalar(8, 60, 0), Scalar(22, 120, 255), threshed);

		erode(threshed, threshed, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		dilate(threshed, threshed, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		dilate(threshed, threshed, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		erode(threshed, threshed, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

		//It is now time for line doings.

		Scalar pixel;
		std::vector<Point> points;
		
		for(int i=0; i<threshed.cols; i++){
			for(int j=0; j<threshed.rows; j++){
				pixel = threshed.at<Scalar>(Point(i, j));
				if(pixel[2] !=0){
					points.push_back(Point(i, j));
				}
			}
		}

		Vec4f outVector;
		fitLine(points, outVector, CV_DIST_L2, 0, 0.01, 0.01);
		
		double m = max(threshed.rows, threshed.cols);
		
		Point start;
		start.x = outVector[2] - m*outVector[0];
		start.y = outVector[3] - m*outVector[1];

		Point end;
		end.x = outVector[2] + m*outVector[0];
		end.y = outVector[3] + m*outVector[1];

		circle(frame, Point(outVector[2], outVector[3]), 10, Scalar(255, 0, 0), 1, 8, 0);
		line(frame, start, end, Scalar(0, 0, 255), 1, 8, 0 );
	
		if(count % 100 ==0)
			cout<<"WRITING FRAME "<<count<<endl;

		count++;
		out.write(frame);
	}

	in.release();
	cout<<argv[1]<<endl;
}
