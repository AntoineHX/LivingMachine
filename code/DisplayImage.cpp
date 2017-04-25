
#include <cv.h> 
#include <highgui.h>
#include <cxcore.h>

 
//callback for trackbar. nothing to be done here     
void on_trackbar( int, void* )
{
}

int main(int argc, char* argv[])
{
	int height,width,step,channels;  //parameters of the image we are working on
	
    // Open capture device. 0 is /dev/video0, 1 is /dev/video1, etc.
    CvCapture* capture = cvCaptureFromCAM( 0 );
    
    if( !capture )
    {
            fprintf( stderr, "ERROR: capture is NULL \n" );
            getchar();
            return -1;
    }
    // grab an image from the capture
    IplImage* frame = cvQueryFrame( capture );
    
    // Create a window in which the captured images will be presented
    cvNamedWindow( "Camera", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "HSV", CV_WINDOW_AUTOSIZE );
    
    // get the image data
      height    = frame->height;
      width     = frame->width;
      step      = frame->widthStep;
      
     // capture size - 
    CvSize size = cvSize(width,height);
    
    // Initialize different images that are going to be used in the program
    IplImage*  hsv_frame    = cvCreateImage(size, IPL_DEPTH_8U, 3); // image converted to HSV plane
    IplImage*  thresholded   = cvCreateImage(size, IPL_DEPTH_8U, 1);
    
  
    while( 1 )
    {   

        // Get one frame
        frame = cvQueryFrame( capture );
        
        if( !frame )
        {
                fprintf( stderr, "ERROR: frame is null...\n" );
                getchar();
                break;
        }
 
        // Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
        cvCvtColor(frame, hsv_frame, CV_BGR2HSV);
 
        cvInRangeS(frame, cvScalar(255, 100, 230), cvScalar(255, 200, 255), thresholded);
           
         cvShowImage( "Camera", frame ); // Original stream with detected ball overlay
         cvShowImage( "HSV", hsv_frame); // Original stream in the HSV color space
         cvShowImage( "After Color Filtering", thresholded ); // The stream after color filtering
     
        if( (cvWaitKey(10) ) >= 0 ) break;
    }

     // Release the capture device housekeeping
     cvReleaseCapture( &capture );
     cvDestroyWindow( "mywindow" );
     return 0;
   }
