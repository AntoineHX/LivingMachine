#include <stdio.h>
#include <stdlib.h>
#include <cv.h> 
#include <highgui.h>
//#include <opencv2/highgui.hpp> //Pour le cvRound
//#include "opencv2/videoio/videoio_c.h" //Pour le CvCapture*

//#include <cxcore.h>
//#include <SFML/Window.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#define CONFIG
//#define SFML

#define KIRBY
//#define ETOILE

//ATTENTION AFFICHAGE OPENCV INCOMPATIBLE AVEC AFFICHAGE SFML

/*Headers*/
void controle_moteur(int vecX, int vecY, int rayon);
int limite_moteur(int val_pwm);

void config(int* LowH, int* HighH, int* LowS, int* HighS, int* LowV, int* HighV); //Affiche le panneau de configuration de tracking avec les arguments comme valeur de base
void affichage_config(IplImage* frame, IplImage* HSV, IplImage* Binaire); //Affiche le flux vidéos et ses différent traitements
void Affichage_Tracking(IplImage* frame, int posX, int posY, int width, int height); //Dessine les informations de tracking sur frame

void Position_moy(IplImage* Binaire, int* posX, int * posY); //Effectue le baricentre des pixels d'une image binaire pour obtenir la postion de l'objet
void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV); //Effectue une binarisation de frame en fonction des bornes HSV

int image_CV2SFML(IplImage* imcv, sf::Image imsf); //Construction de imsf (RGBA) à partir de imcv (BGR), avec alpha constant (=1)

int main(int argc, char* argv[])
{
	int height,width,step,channels;  //parameters of the image we are working on
	int posX, posY; //Position objet
	int boucle;
#ifdef SFML

	//Initialisation SFML
	
	sf::Texture txFlux;
	sf::Sprite spFlux;
	sf::Image imFlux;
   	sf::Event event;
	
	//Création de la fenetre principale
	sf::RenderWindow window(sf::VideoMode(800, 600), "KirbyTrack");
#endif

    // Open capture device. 0 is /dev/video0, 1 is /dev/video1, etc.
    CvCapture* capture = cvCaptureFromCAM( 0 );
    
    if( !capture ){
            printf("ERROR: capture is NULL \n" );
            return EXIT_FAILURE;
    }
    

    // grab an image from the capture
    IplImage* frame = cvQueryFrame( capture );
    
    // get the image data
      height    = frame->height;
      width     = frame->width;
      step      = frame->widthStep;
      
     // capture size - 
    CvSize size = cvSize(width,height);
    
#ifdef SFML
	//Intialisation de la texture
	if (!txFlux.create(width, height)){
		printf("Erreur création texture\n");
		return EXIT_FAILURE;
	}
#endif

    // Initialize different images that are going to be used in the program
    IplImage*  hsv_frame    = cvCreateImage(size, IPL_DEPTH_8U, 3); // image converted to HSV plane
    IplImage*  threshold   = cvCreateImage(size, IPL_DEPTH_8U, 1);
    
    //Controle couleur
#ifdef KIRBY
	//Setup Kirby
	 int iLowH = 139;
	 int iHighH = 179;

	 int iLowS = 48; 
	 int iHighS = 255;

	 int iLowV = 101;
	 int iHighV = 255;
#endif
#ifdef ETOILE
	//Setup Etoile
	int iLowH = 20;
	int iHighH = 30;

	int iLowS = 100; 
	int iHighS = 255;

	int iLowV = 100;
	int iHighV = 255;
#endif	

#ifdef CONFIG
	//Affichage du panneau de config
	config(&iLowH, &iHighH, &iLowS, &iHighS, &iLowV, &iHighV);

	boucle = 1;
#endif
	 
    while(boucle)//while(window.isOpen())
    {  

#ifdef SFML
	boucle = window.isOpen();

	// on inspecte tous les évènements de la fenêtre qui ont été émis depuis la précédente itération
        while (window.pollEvent(event))
        {
            // évènement "fermeture demandée" : on ferme la fenêtre
            if (event.type == sf::Event::Closed)
                window.close();
        }
#endif

        // Get one frame
        frame = cvQueryFrame( capture );
        
        if( !frame ){
                printf("ERROR: frame is null...\n" );
                break;
        }

 	//Binarisation du flux vidéo
	traitement(frame, hsv_frame, threshold, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);
        
        // Calculate the moments to estimate the position of the ball 
        Position_moy(threshold, &posX, &posY);
        
	//Dessine les informations de tracking sur frame
	Affichage_Tracking(frame, posX, posY, width, height);

#ifdef SFML
	//Affichage SFML
	/* Clear the screen */
        window.clear(sf::Color::Black);	

	//Conversion de la frame en image smfl  
	if(image_CV2SFML(frame, imFlux)){
		printf("Erreur conversion OpenCV-SFML\n");
		break;
	}
     
	spFlux.setTexture(txFlux);

   	window.draw(spFlux);
	
	/* Update the window */
        //window.display();

	//sfSprite_destroy(sprite);
    	//sfTexture_destroy(texture);
#endif

	//controle_moteur(posX-width/2, posY-height/2, height/6); //Envoie commande moteur

#ifdef CONFIG
	affichage_config(frame, hsv_frame, threshold); //Affichage du flux vidéo et de ses traitements
	
        if( (cvWaitKey(10) ) >= 0 ) break; //Arret capture
#endif
    }
    
	//cvWaitKey(0); //Fin programme
	
     // Release the capture device housekeeping
     cvReleaseCapture( &capture );
     
     cvReleaseImage(&threshold);
     cvReleaseImage(&hsv_frame);
     cvReleaseImage(&frame);
     
     return EXIT_SUCCESS;
   }

/*On se rapproche de (vecX, vecY) si la position se situe en dehors d'un cercle centre sur la camera*/
void controle_moteur(int vecX, int vecY, int rayon){

	int val_pwm[2];

	/*Lecture valeur*/
	FILE* fichier = NULL;
	fichier = fopen("/dev/ttyACM0","r");
	if(fichier==NULL){
		printf("Erreur ouverture fichier\n");
		return ;
	}
	
	fscanf(fichier,"%d,%d",&val_pwm[0],&val_pwm[1]);
	
	fclose(fichier);

	/*Ecriture nouvelle valeur*/
	fichier = fopen("/dev/ttyACM0","w");
	if(fichier==NULL){
		printf("Erreur ouverture fichier\n");
		return ;
	}
	double norme = 1.0*vecX*vecX + 1.0*vecY*vecY;
	
	if (norme > rayon*rayon){
		if(vecX >= vecY && limite_moteur(val_pwm[0])){ /*Ecart sur x plus important*/
			fprintf(fichier,"%d,%d",val_pwm[0]++,val_pwm[1]);
		}
		else if(vecX <= vecY && limite_moteur(val_pwm[1])){	/*Ecart sur y plus important*/
			fprintf(fichier,"%d,%d",val_pwm[0],val_pwm[1]++);
		}
	}

	fclose(fichier);
	return;
}

/*Verifie que les valeurs envoyees aux moteurs sont correctes*/
int limite_moteur(int val_pwm){
	int MAX_PWM = 255;
	if (val_pwm > MAX_PWM || val_pwm < 0){
		return 0;
	}
	else{
		return 1;
	}
}
/*
int image_CV2SFML(IplImage* imcv, sf::Image imFlux){
	
}
*/

void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV){ //Effectue une binarisation de frame en fonction des bornes HSV

	// Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
        cvCvtColor(frame, HSV, CV_BGR2HSV);
	
	//Blur
	cvSmooth( HSV, HSV, CV_GAUSSIAN, 15, 0,0,0); //suppression des parasites par flou gaussien

	//Binarisation

	//CvScalar valinf={LowH,LowS,LowV};
	//CvScalar valsup={HighH,HighS,HighV};

        //cvInRangeS(HSV, valinf,valsup, Binaire);
	cvInRangeS(HSV, CvScalar(LowH,LowS,LowV),CvScalar(HighH,HighS,HighV), Binaire);
      
        //cvSmooth( Binaire, Binaire, CV_GAUSSIAN, 9, 9 ); //Legère suppression des parasites
}

void Position_moy(IplImage* Binaire, int* posX, int * posY){ //Effectue le baricentre des pixels d'une image binaire pour obtenir la postion de l'objet

	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments)); 

	cvMoments(Binaire, moments, 1); 
        // The actual moment values 
        double moment10 = cvGetSpatialMoment(moments, 1, 0); 
        double moment01 = cvGetSpatialMoment(moments, 0, 1); 
        double area = cvGetCentralMoment(moments, 0, 0);
        
        *posX = moment10/area;
        *posY = moment01/area;

	free(moments);
}

void config(int* LowH, int* HighH, int* LowS, int* HighS, int* LowV, int* HighV){ //Affiche le panneau de configuration de tracking avec les arguments comme valeur de base

	cvNamedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	 //Create trackbars in "Control" window
	 cvCreateTrackbar("LowH", "Control", LowH, 179,NULL); //Hue (0 - 179)
	 cvCreateTrackbar("HighH", "Control", HighH, 179,NULL);

	 cvCreateTrackbar("LowS", "Control", LowS, 255,NULL); //Saturation (0 - 255)
	 cvCreateTrackbar("HighS", "Control", HighS, 255,NULL);

	 cvCreateTrackbar("LowV", "Control", LowV, 255,NULL); //Value (0 - 255)
	 cvCreateTrackbar("HighV", "Control", HighV, 255,NULL);
}

void affichage_config(IplImage* frame, IplImage* HSV, IplImage* Binaire){ //Affiche le flux vidéos et ses différent traitements
	
	// Create a window in which the captured images will be presented
	cvNamedWindow( "HSV", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "Binaire", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "Camera", CV_WINDOW_AUTOSIZE );

        cvShowImage( "HSV", HSV); // Original stream in the HSV color space
        cvShowImage( "Binaire", Binaire); // The stream after color filtering
	cvShowImage( "Camera", frame ); // Flux caméra avec tracking objet
}

void Affichage_Tracking(IplImage* frame, int posX, int posY, int width, int height){ //Dessine les informations de tracking sur frame

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
}
