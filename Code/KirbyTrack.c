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

//#define CONFIG
#define SFML

#define KIRBY
//#define ETOILE

//ATTENTION AFFICHAGE OPENCV INCOMPATIBLE AVEC AFFICHAGE SFML

/*Headers*/
void maj_angle(int vecX, int vecY, int rayon, int* angle); //Met à jour l'angle selon la distance CentreCamera - Cible
int ajust_pos(int pos, int ref)
void controle_moteur(int* angle);//Envoie les angles au moteur
int limite_moteur(int val_pwm);//Verifie que les valeurs envoyees aux moteurs sont correctes

void config(int* LowH, int* HighH, int* LowS, int* HighS, int* LowV, int* HighV); //Affiche le panneau de configuration de tracking avec les arguments comme valeur de base
void affichage_config(IplImage* frame, IplImage* HSV, IplImage* Binaire); //Affiche le flux vidéos et ses différent traitements
void Affichage_Tracking(IplImage* frame, int posX, int posY, int width, int height); //Dessine les informations de tracking sur frame

void Position_moy(IplImage* Binaire, int* posX, int * posY); //Effectue le baricentre des pixels d'une image binaire pour obtenir la postion de l'objet
void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV); //Effectue une binarisation de frame en fonction des bornes HSV

int image_CV2SFML(IplImage* imcv, sf::Image imFlux); //Construction de imsf (RGBA) à partir de imcv (BGR), avec alpha constant (=1)


int main(int argc, char* argv[])
{
	//Initialisations 
	int height,width,step,channels;  //parameters of the image we are working on
	int posX, posY; //Position objet
	int boucle;

	int angle[2] = {100,100};
	int vecX, vecY;


#ifdef SFML
	//Initialisation SFML
	
	sf::Texture txFlux;
	sf::Sprite spFlux;
	sf::Image imFlux;
  sf::Event event;
	
#endif

  //Ouverture flux camera
  CvCapture* capture = cvCaptureFromCAM( 0 );
    
  if( !capture ){
  	printf("ERROR: capture is NULL \n" );
  	exit(EXIT_FAILURE);
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
	//Création de la fenetre principale
	sf::RenderWindow window(sf::VideoMode(width, height), "KirbyTrack");
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
	 
<<<<<<< HEAD
    while(boucle)
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
			perror("ERROR: frame is null...");
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
	/*if(image_CV2SFML(frame, imFlux)){
		printf("Erreur conversion OpenCV-SFML\n");
		break;
	}
     	*/
	//Enregistrement de la frame openCV
	cvSaveImage("temp.jpg", frame);
	
	//Chargement de la frame en texture SFML
	if (!txFlux.loadFromFile("temp.jpg")){
		printf("Erreur chargement image SFML\n" );
                break;
	}

	spFlux.setTexture(txFlux);


		window.draw(spFlux);
	

	/* Update the window */
        window.display();

#endif

		//Mouvements moteurs
		printf("-PREMAJ_ANGLE...: %d %d\n",width,height);

		maj_angle(ajust_pos(posX-width/2,width), ajust_pos(posY-height/2,height), height/10, angle);
		controle_moteur(angle); 
		cvWaitKey(50);

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


void maj_angle(int vecX, int vecY, int rayon, int* angle){
	//On ajustera coeff fonction du rayon. Si la cible est à une distance 5*r, il sera 5x plus rapide que s'il était à 1*r

	int coeffx, coeffy, l0, l1;

	printf("-MAJ_ANGLE...Valeur maj_angle arguments : %d %d %d\n\tAnciens angles : %d %d\n\t",vecX,vecY,rayon,angle[0],angle[1]);


	//Ajout d'un angle moteur pondéré par la distance
	coeffx = vecX/rayon;
	coeffy = vecY/rayon;
	angle[0] += coeffx;
	angle[1] += coeffy;

	//Majoration - minoration des angles moteurs
	l0 = limite_moteur(angle[0]);
	l1 = limite_moteur(angle[1]);
	if (l0 != 0) angle[0] = l0;
	if (l1 != 0) angle[1] = l1;

	printf("Nouveaux angles : %d %d\n",angle[0],angle[1]);
}

int ajust_pos(int pos, int ref){
	if (pos > ref) return 0;
	else return pos;
}

int limite_moteur(int val_pwm){
	int MAX_PWM = 130, MIN_PWM = 30;
	if (val_pwm > MAX_PWM){ 
		return MAX_PWM;
	}
	else if (val_pwm < MIN_PWM){
		return MIN_PWM;
	}
	else{
		return 0;
	}
}

void controle_moteur(int* angle){

	//Ouverture port serie
	FILE* fichier = NULL;
	fichier = fopen("/dev/ttyACM0","w");
	if(fichier==NULL){
		printf("Erreur ouverture fichier\n");
		perror("fopen failed for /dev/ttyACM0" );
		exit( EXIT_FAILURE );
	}

	//Ecriture angles
	fprintf(fichier,"%d\n",angle[0]);
	fprintf(fichier,"%d\n",angle[1]);

	//Fermeture
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


int image_CV2SFML(IplImage* imcv, sf::Image imFlux){
	
	int R, G, B;
	int w = imcv->widthStep;
	char* ptr = imcv->imageData;

	imFlux.create(imcv->width,imcv->height, NULL); //Initialise une image vide

	for( int y=0; y<imcv->height; y++ ) { 
        	//uchar* ptr = (uchar*) ( imcv->imageData + y * imcv->widthStep );
        	for( int x=0; x<imcv->width; x++ ) { 
            		//Recupération du pixel
			B = ptr[y*w + 3*x];
			G = ptr[y*w + 3*x + 1]; 
			R = ptr[y*w + 3*x + 2];

			//Ecriture du pixel associé
			imFlux.setPixel(x,y,sf::Color(R,G,B,1)); //Alpha channel = 1
        	}
    	}

	return 0;
}


void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV){ //Effectue une binarisation de frame en fonction des bornes HSV

	// Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
        cvCvtColor(frame, HSV, CV_BGR2HSV);
	
	//Blur
	cvSmooth( HSV, HSV, CV_GAUSSIAN, 15, 0,0,0); //suppression des parasites par flou gaussien

	//Binarisation

	CvScalar valinf={LowH,LowS,LowV};
	CvScalar valsup={HighH,HighS,HighV};

	//En cas d'erreur sur les trois ligne précédentes
        cvInRangeS(HSV, valinf,valsup, Binaire);

	//cvInRangeS(HSV, CvScalar(LowH,LowS,LowV),CvScalar(HighH,HighS,HighV), Binaire);
      
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
