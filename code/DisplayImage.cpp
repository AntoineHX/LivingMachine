#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{

		
		VideoCapture cap(0); // open the default camera
    		if(!cap.isOpened())  // check if we succeeded
   			return -1;
		Mat frame;
		
		cvNamedWindow("tests",CV_WINDOW_AUTOSIZE);
		while(1){
				
				cap >> frame; // get a new frame from camera
				
				imshow("tests", frame);
				if(waitKey(30) >= 0) break; //Arrete la capture

		}
		cv::waitKey(0); // Termine le programme
		
		return 0;
}
