#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h> 
#include <highgui.h>
//#include <cxcore.h>

//#include <SFML/Window.hpp>

int main(int argc, char* argv[])
{
	int height,width,step,channels;  //parameters of the image we are working on
	int posX, posY; //Position objet
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments)); //Variable moyenne position
	
    // Open capture device. 0 is /dev/video0, 1 is /dev/video1, etc.
    CvCapture* capture = cvCaptureFromCAM( 0 );
    
    if( !capture ){
            printf("ERROR: capture is NULL \n" );
            return -1;
    }
    
    // grab an image from the capture
    IplImage* frame = cvQueryFrame( capture );
    
    // Create a window in which the captured images will be presented
    cvNamedWindow( "Camera", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "HSV", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "Binaire", CV_WINDOW_AUTOSIZE );

    //sf::Window window;
    //window.create(sf::VideoMode(800, 600), "My window",sf::Style::Default);
    
    // get the image data
      height    = frame->height;
      width     = frame->width;
      step      = frame->widthStep;
      
     // capture size - 
    CvSize size = cvSize(width,height);
    
    // Initialize different images that are going to be used in the program
    IplImage*  hsv_frame    = cvCreateImage(size, IPL_DEPTH_8U, 3); // image converted to HSV plane
    IplImage*  threshold   = cvCreateImage(size, IPL_DEPTH_8U, 1);
    
    //Controle couleur
	cvNamedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	//Setup Kirby
	 int iLowH = 139;
	 int iHighH = 179;

	 int iLowS = 48; 
	 int iHighS = 255;

	 int iLowV = 101;
	 int iHighV = 255;
	CvScalar valinf={iLowH,iLowS,iLowV};
	CvScalar valsup={iHighH,iHighS,iHighV};


	 //Create trackbars in "Control" window
	 cvCreateTrackbar("LowH", "Control", &iLowH, 179,NULL); //Hue (0 - 179)
	 cvCreateTrackbar("HighH", "Control", &iHighH, 179,NULL);

	 cvCreateTrackbar("LowS", "Control", &iLowS, 255,NULL); //Saturation (0 - 255)
	 cvCreateTrackbar("HighS", "Control", &iHighS, 255,NULL);

	 cvCreateTrackbar("LowV", "Control", &iLowV, 255,NULL); //Value (0 - 255)
	 cvCreateTrackbar("HighV", "Control", &iHighV, 255,NULL);

	  
    while( 1 )
    {   

        // Get one frame
        frame = cvQueryFrame( capture );
        
        if( !frame ){
                printf("ERROR: frame is null...\n" );
                break;
        }
 
        // Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
        cvCvtColor(frame, hsv_frame, CV_BGR2HSV);
	
	//Blur
	cvSmooth( hsv_frame, hsv_frame, CV_GAUSSIAN, 15, 0,0,0); //suppression des parasites par flou gaussien

	//Binarisation
        cvInRangeS(hsv_frame, valinf,valsup, threshold);
      
        //cvSmooth( threshold, threshold, CV_GAUSSIAN, 9, 9 ); //Legère suppression des parasites
        
        // Calculate the moments to estimate the position of the ball 
        
        cvMoments(threshold, moments, 1); 
        // The actual moment values 
        double moment10 = cvGetSpatialMoment(moments, 1, 0); 
        double moment01 = cvGetSpatialMoment(moments, 0, 1); 
        double area = cvGetCentralMoment(moments, 0, 0);
        
        posX= moment10/area;
        posY= moment01/area;
        
	//Affichage zone suivie objet
        cvCircle(frame, cvPoint(width/2,height/2), height/6, CV_RGB(0, 255, 0), 4, 8, 0 );

	if(posX<5&&posY<5){ //Si aucun objet spotted, pointeur rouge au centre
		posX=width/2;
		posY=height/2;
		cvLine(frame, cvPoint(posX-20,posY), cvPoint(posX+20,posY), CV_RGB(255, 0, 0), 4, 8, 0 );
	cvLine(frame, cvPoint(posX,posY-20), cvPoint(posX,posY+20), CV_RGB(255, 0, 0), 4, 8, 0 );
	}
	else{ //Objet spotted
	//Affichage position de l'objet
	cvLine(frame, cvPoint(posX-20,posY), cvPoint(posX+20,posY), CV_RGB(0, 0, 255), 4, 8, 0 );
	cvLine(frame, cvPoint(posX,posY-20), cvPoint(posX,posY+20), CV_RGB(0, 0, 255), 4, 8, 0 );
        }
         cvShowImage( "Camera", frame ); // Original stream with detected ball overlay
         cvShowImage( "HSV", hsv_frame); // Original stream in the HSV color space
         cvShowImage( "Binaire", threshold); // The stream after color filtering
     
	controle_moteur(posX-width/2, posY-height/2, height/6); //Envoie commande moteur

        if( (cvWaitKey(10) ) >= 0 ) break; //Arret capture
    }
    
	cvWaitKey(0); //Fin programme
	
     // Release the capture device housekeeping
     cvReleaseCapture( &capture );
     
     cvReleaseImage(&threshold);
     
     return 0;
   }

int* controle_moteur(int vecX, int vecY, int rayon){

	FILE* fichier = fopen("commande.txt",w);

	double norme = sqrt(vecX*vecX + vecY*vecY);
	
	if (norme > rayon){
		if(vecX/norme >= 0.5){
			on ajoute 1 sur XXX
		}
		else{
			on ajoute 1 sur YYY
		}

	}


	fclose(fichier);

}
