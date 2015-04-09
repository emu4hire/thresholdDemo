#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int distCode=0;

int main(int argc, char ** argv){
	VideoCapture in;
	VideoWriter out;

	if(argc >1){
		int num =1;

		while(num < argc){
			string input = argv[num];

			if(input == "-l2" || input == "-L2")
				distCode = 0;
			else if(input == "-l12" || input == "L12")
				distCode = 1;
			else if(input == "-fair" || input == "-FAIR")
				distCode =2;
			else if(input == "-welsch" || input == "-WELSCH")
				distCode =3;
			else if(input == "-huber" || input == "-HUBER")
				distCode = 4;
			else{		
				in.open(argv[num]);
				const string source = argv[num];
				string::size_type pAt = source.find_last_of('.');
				string NAME = source.substr(0, pAt);

				if(distCode == 0)
					NAME = NAME + "_L2.avi";
				else if(distCode == 1)
					NAME = NAME + "_L12.avi";
				else if(distCode == 2)
					NAME = NAME + "_FAIR.avi";
				else if(distCode == 3)
					NAME = NAME + "_WELSCH.avi";
				else if(distCode == 4)
					NAME = NAME + "_HUBER.avi";
				else
					NAME = NAME + "_EDITED.avi";
	
				cerr<<NAME<<endl;	
				out.open(NAME, CV_FOURCC('D', 'I', 'V', 'X') , in.get(CV_CAP_PROP_FPS), 
					Size( (int) in.get(CV_CAP_PROP_FRAME_WIDTH), (int) in.get(CV_CAP_PROP_FRAME_HEIGHT)), true);
			}
			num++;
		}
	}

	else
		exit(1);

	if(!in.isOpened()){
		cerr<<"Was unable to open video input."<<endl;
		exit(1);
	}
	
	if(!out.isOpened()){
		cerr<<"Was unable to open video output."<<endl;
		exit(1);
	}
	
	int frameNum=0;
	while(true){
		Mat frame(in.get(CV_CAP_PROP_FRAME_HEIGHT), in.get(CV_CAP_PROP_FRAME_WIDTH), CV_8UC3);
		Mat color(frame.rows, frame.cols, CV_8UC3);
		
		if(!in.read(frame)){
			cerr<<"CANNOT READ FRAME.  VIDEO STREAM OVER."<<endl;
			break;
		}

		cvtColor(frame, color, CV_BGR2HSV, 3);

		Vec3b pixel;
		Vec4f outLineC, outLineE;
		vector<Point> pointsC;
		vector<Point> pointsE;

		for(int i=0; i<color.rows; i++){
			for(int j=0; j<color.cols; j++){
				pixel = color.at<Vec3b>(Point(j,i));	
				if( (pixel[0] > 9 && pixel[0] < 25) && (pixel[1] > 0 && pixel[1] < 169) && (pixel[2] > 169 && pixel[2] < 255)){
					pointsC.push_back(Point(j, i));
				}
				if( (pixel[0] > 36 && pixel[0] < 85) && (pixel[1] > 5 && pixel[1] < 255) && (pixel[2] >208 && pixel[2] <255)){
					pointsE.push_back(Point(j, i));
				}
			}
		}
		
                for(int k=0; k < pointsC.size(); k++){
                        circle(frame, pointsC[k], 1, Scalar(255, 0, 0), -1,CV_AA, 0);
                }

		for(int k=0; k<pointsE.size(); k++){
			circle(frame, pointsE[k], 1, Scalar(0, 0, 255), -1, CV_AA, 0);
		}

		double m = max(color.rows, color.cols);
		Point Cp1, Cp2, Ep1, Ep2;
	
		if(pointsC.size() >0){
	
			//Simplest, least squares method (RED).
			if(distCode == 0){
				fitLine(pointsC, outLineC, CV_DIST_L2, 0, 0.01, 0.01);
				fitLine(pointsE, outLineE, CV_DIST_L2, 0, 0.01, 0.01);
				putText(frame, "DIST L2 - Least Squares", Point(0, frame.rows-100), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 255, 255), 1, CV_AA, false);
			}
			else if(distCode == 1){
				fitLine(pointsC, outLineC, CV_DIST_L12, 0, 0.01, 0.01);
				fitLine(pointsE, outLineE, CV_DIST_L12, 0, 0.01, 0.01);
				putText(frame, "DIST L12", Point(0, frame.rows-100), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 255, 255), 1, CV_AA, false);
			}
			else if(distCode == 2){
				fitLine(pointsC, outLineC, CV_DIST_FAIR, 0, 0.01, 0.01);
				fitLine(pointsE, outLineE, CV_DIST_FAIR, 0, 0.01, 0.01);
				putText(frame, "DIST FAIR", Point(0, frame.rows-100), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 255, 255), 1, CV_AA, false);
			}
			else if(distCode == 3){
				fitLine(pointsC, outLineC, CV_DIST_WELSCH, 0, 0.01, 0.01);
				fitLine(pointsE, outLineE, CV_DIST_WELSCH, 0, 0.01, 0.01);
				putText(frame, "DIST WELSCH", Point(0, frame.rows-100), FONT_HERSHEY_SIMPLEX,2,  Scalar(255, 255, 255), 1, CV_AA, false);
			}
			else if(distCode == 4){
				fitLine(pointsC, outLineC, CV_DIST_HUBER, 0, 0.01, 0.01);
				fitLine(pointsE, outLineE, CV_DIST_HUBER, 0, 0.01, 0.01);
				putText(frame, "DIST HUBER", Point(0, frame.rows-100), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 255, 255), 1, CV_AA, false);
			}
		}

		Cp1.x = outLineC[2] - m*outLineC[0];
		Cp1.y = outLineC[3] - m*outLineC[1];
		Cp2.x = outLineC[2] + m*outLineC[0];
		Cp2.y = outLineC[3] + m*outLineC[1];

		Ep1.x = outLineE[2] - m*outLineE[0];
		Ep1.y = outLineE[2] - m*outLineE[1];
		Ep1.x = outLineE[2] - m*outLineE[0];
		Ep1.y = outLineE[2] - m*outLineE[1];
		
		
		line(frame, Cp1, Cp2, Scalar(0, 255, 0), 3, CV_AA, 0);
		line(frame, Ep1, Ep2, Scalar(255, 0, 255), 3, CV_AA,0);
	
		out.write(frame);

		if(frameNum % 100 == 0)
			cout<<"FRAME "<<frameNum<<endl;

		frameNum++;

	}
	
	out.release();

}
