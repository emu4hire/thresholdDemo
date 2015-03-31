#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char ** argv){
	VideoCapture in;
	VideoWriter out;
	VideoWriter test;

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

		bool success = in.read(frame);

		if(!success){
			cout<<"VIDEO STREAM COMPLETE"<<endl;
			break;
		}
	
		//Lets do some thresholding.
		
		cvtColor(frame, colorImg, COLOR_BGR2HSV);
		
		for(int i=0; i<colorImg.rows; i++){
			for(int j=0; j<colorImg.cols; j++){

				Vec3i & pixel = colorImg.at<Vec3i>(Point(j, i));

				if( (pixel[0] > 8 && pixel[0] < 22) && (pixel[1] > 60 && pixel[1] < 120 ) && (pixel[2] > 0 && pixel[2] <255)){
					pixel[0] = 255;
					pixel[1] = 0;
					pixel[2] = 0;
				}	
				else{
					pixel[0] = 0;
					pixel[1] = 0;
                                        pixel[2] = 0;

				}
				colorImg.at<Vec3i>(Point(j, i)) = pixel;

			}
		}
		
		//It is now time for line doings.
		Vec3i pixel;
		vector<Point> points;
		
		for(int i=0; i<colorImg.cols; i++){
			for(int j=0; j<colorImg.rows; j++){
				pixel = colorImg.at<Vec3i>(i, j);
				if(pixel[0] !=0){
					points.push_back(Point(i, j));
				}
			}
		}

		
		Point * pointArr = new Point[points.size()];
		Mat pointMat = Mat(1, points.size(), CV_32F, pointArr);
		Vec4f outVector;

		fitLine(points, outVector, CV_DIST_L2, 0, 0.01, 0.01);

		float d = sqrt((double)outVector[0]*outVector[0] + (double)outVector[1]*outVector[1]);
		outVector[0] /= d;
		outVector[1] /= d;
		float m = (float) (frame.cols + frame.rows);
		
		Point start;
		start.x = outVector[2] - m*outVector[0];
		start.y = outVector[3] - m*outVector[1];

		Point end;
		end.x = outVector[2] + m*outVector[0];
		end.y = outVector[3] + m*outVector[1];

		for(int i=0; i<points.size(); i++){
			circle(frame, points[i], 2, Scalar(255, 0, 0), CV_FILLED, CV_AA, 0);
		}	
	
		circle(frame, Point(outVector[2], outVector[3]), 10, Scalar(255, 0, 0), 1, 8, 0);
		line(frame, start, end, Scalar(0, 0, 255), 3, CV_AA, 0 );
	
		if(count % 100 ==0)
			cout<<"WRITING FRAME "<<count<<endl;

		count++;
		out.write(frame);
	}

	in.release();
	cout<<argv[1]<<endl;
}
