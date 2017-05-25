#include "fonction.h"


void maj_angle(int vecX, int vecY, int rayon, double* angle){
	//On ajustera coeff fonction du rayon. Si la cible est à une distance 5*r, il sera 5x plus rapide que s'il était à 1*r

	double coeffx, coeffy;
	int l0, l1;

	//printf("-MAJ_ANGLE...Valeur maj_angle arguments : %d %d %d\n\tAnciens angles : %d %d\n\t",vecX,vecY,rayon,(int)angle[0],(int)angle[1]);


	//Ajout d'un angle moteur pondéré par la distance
	coeffx = -0.2*vecX/rayon;
	coeffy = 0.2*vecY/rayon;
	angle[0] += coeffx;
	angle[1] += coeffy;

	//Majoration - minoration des angles moteurs
	l0 = limite_moteur(angle[0]);
	l1 = limite_moteur(angle[1]);
	if (l0 != 0) angle[0] = l0;
	if (l1 != 0) angle[1] = l1;

	//printf("Nouveaux angles : %lf %lf %d %d\n",angle[0],angle[1],(int)angle[0],(int)angle[0]);
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

void controle_moteur(double* angle){

	//Ouverture port serie
	FILE* fichier = NULL;
	fichier = fopen("/dev/ttyACM0","w");
	if(fichier==NULL){
		printf("Erreur ouverture fichier\n");
		perror("fopen failed for /dev/ttyACM0" );
		exit( EXIT_FAILURE );
	}

	//Ecriture angles
	fprintf(fichier,"%d\n",(int)angle[0]);
	fprintf(fichier,"%d\n",(int)angle[1]);

	//Fermeture
	fclose(fichier);
	return;
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
/*
	CvScalar valinf={(double)LowH,(double)LowS,(double)LowV};
	CvScalar valsup={(double)HighH,(double)HighS,(double)HighV};

        cvInRangeS(HSV, valinf,valsup, Binaire);
*/	
	//En cas d'erreur sur les trois ligne précédentes
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
        cvCircle(frame, cvPoint(width/2,height/2), height*JEU, CV_RGB(0, 255, 0), 4, 8, 0 );

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


CvHaarClassifierCascade* init_cascade(){
	// Create a new Haar classifier
	CvHaarClassifierCascade* cascade = 0;
	const char* cascade_name = "haarcascade_frontalface_alt.xml";

	
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );

	// Check whether the cascade has loaded successfully. Else report and error and quit
	if( !cascade ){
		fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
		perror(" ");
		return NULL;
	}
	return cascade;
}

// Function to detect and draw any faces that is present in an image
void detect_and_draw( IplImage* img, CvHaarClassifierCascade* cascade, pface* tab_face)
{

	// Create memory for calculations
	static CvMemStorage* storage = 0;
	int scale = 1;

	// Create a new image based on the input image
	IplImage* temp = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );

	// Create two points to represent the face locations
	CvPoint pt1, pt2;
	int i;

	// Allocate the memory storage
	storage = cvCreateMemStorage(0);

	// Create a new named window with title: result
	cvNamedWindow( "result", 1 );

	// Clear the memory storage which was used before
	cvClearMemStorage( storage );

	// Find whether the cascade is loaded, to find the faces. If yes, then:
	if( cascade ){

		// There can be more than one face in an image. So create a growable sequence of faces.
		// Detect the objects and store them in the sequence
		CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,1.1, 2, 0, cvSize(60, 60),cvSize(500, 500));

		// Loop the number of faces found.
		for( i = 0; i < (faces ? faces->total : 0); i++ ){
			// Create a new rectangle for drawing the face
			CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

			// Find the dimensions of the face,and scale it if necessary
			pt1.x = r->x*scale;
			pt2.x = (r->x+r->width)*scale;
			pt1.y = r->y*scale;
			pt2.y = (r->y+r->height)*scale;

			// Draw the rectangle in the input image
			cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );

			if(i < MAX_FACE){
				tab_face[i]->point.x = (pt1.x + pt2.x)/2;
				tab_face[i]->point.y = (pt1.y + pt2.y)/2;
				tab_face[i]->largeur = r->width;

				printf("VALEURS FACES : %d %d %d\n",tab_face[i]->point.x,tab_face[i]->point.y,tab_face[i]->largeur);
			}
		}
	}

	// Show the image in the window named "result"
	cvShowImage( "result", img );

	//free
	cvReleaseImage( &temp );
}
