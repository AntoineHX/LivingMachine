#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
		CvCapture *capture;
    IplImage *image;

		VideoCapture cap(0); // open the default camera
    		if(!cap.isOpened())  // check if we succeeded
   			return -1;
		Mat edges;
		namedWindow("edges",1);
		for(;;)
		{
				Mat frame;
				cap >> frame; // get a new frame from camera
				cvtColor(frame, edges, CV_BGR2GRAY);
				GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
				Canny(edges, edges, 0, 30, 3);
				imshow("edges", edges);
				if(waitKey(30) >= 0) break;
		}

		cv::waitKey(0);
		return 0;
/*
    capture = cvCaptureFromCAM(0);
    
    if (!capture) {
    
       printf("Ouverture du flux vidéo impossible !\n");
       return 1;
    
    }
    
    cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);
    
    while(1){
        image  = cvQueryFrame(capture);
    
        cvShowImage("Test", image);
    }
    return 0;
*/
}
