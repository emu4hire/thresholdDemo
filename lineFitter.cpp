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
	
	int frameNum=0;
	while(true){
		Mat frame(in.get(CV_CAP_PROP_FRAME_HEIGHT), in.get(CV_CAP_PROP_FRAME_WIDTH), CV_8UC3);
		Mat color(frame.rows, frame.cols, CV_8UC3);
		
		in.read(frame);

		cvtColor(frame, color, CV_BGR2HSV, 3);

		Vec3b pixel;
		Vec4f outLine;
		vector<Point> points;

		for(int i=0; i<color.rows; i++){
			for(int j=0; j<color.cols; j++){
				pixel = color.at<Vec3b>(Point(j,i));	
				if( (pixel[0] > 8 && pixel[0] < 22) && (pixel[1] > 22 && pixel[1] < 60)){
					points.push_back(Point(j, i));
				}	
			}
		}
		
                for(int k=0; k < points.size(); k++){
                        circle(frame, points[k], 1, Scalar(0, 0, 0), -1,CV_AA, 0);
                }

		double m = max(color.rows, color.cols);
		Point p1, p2;

		//Simplest, least squares method (RED).
		fitLine(points, outLine, CV_DIST_L2, 0, 0.01, 0.01);
		p1.x = outLine[2] - m*outLine[0];
		p1.y = outLine[3] - m*outLine[1];

		p2.x = outLine[2] + m*outLine[0];
		p2.y = outLine[3] + m*outLine[1];
		
		line(frame, p1, p2, Scalar(0, 0, 255), 3, CV_AA, 0);
	

		//Fair distribution, green
		fitLine(points, outLine, CV_DIST_FAIR, 0, 0.01, 0.01);

                p1.x = outLine[2] - m*outLine[0];
                p1.y = outLine[3] - m*outLine[1];

                p2.x = outLine[2] + m*outLine[0];
                p2.y = outLine[3] + m*outLine[1];

                line(frame, p1, p2, Scalar(0, 255, 0), 3, CV_AA, 0);

		//Wesch's distribution, blue
		fitLine(points, outLine, CV_DIST_WELSCH, 0, 0.01, 0.01);

                p1.x = outLine[2] - m*outLine[0];
                p1.y = outLine[3] - m*outLine[1];

                p2.x = outLine[2] + m*outLine[0];
                p2.y = outLine[3] + m*outLine[1];

                line(frame, p1, p2, Scalar(255,0, 0), 3, CV_AA, 0);



		out.write(frame);

		if(frameNum % 100 == 0)
			cout<<"FRAME "<<frameNum<<endl;

		frameNum++;

	}
	
	out.release();

}
