/**
 * \file      KirbyTrack.c
 * \author    Jacques / Antoine
 * \date       avril - mai 2017
 * \brief      Figure Imposé : Suivi d'un objet coloré.
 *
 * \details   Suivie d'un Kirby (Rose) ou d'une étoile (Jaune) par une caméra avec mode interface utilisateur ou configuration
 * \bug SFML ne supporte qu'un nombre limité de sprite en fonction du PC
 * \bug Affichage OpenCV incompatible avec SFML
 * \todo Optimisation du chargement de la frame en image SFML
 */


#include <stdio.h>
#include <stdlib.h>
#include <cv.h> 
#include <highgui.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
/**
 * \def SFML 
 * \brief Programme en mode affichage interface utilisateur
 * 
 * \def CONFIG
 * \brief Programme en mode configuration couleur
 *
 * \def KIRBY
 * \brief Programme en mode suivi de Kirby (Objet rose)
 * 
 * \def ETOILE
 * \brief Programme en mide suivi de l'étoile (Objet jaune)
 * 
 * \def JEU
 * \brief Coefficient de tolérance pour le suivi d'objet
 */
//#define CONFIG
#define SFML

#define KIRBY
//#define ETOILE

#define JEU 0.15

//ATTENTION AFFICHAGE OPENCV INCOMPATIBLE AVEC AFFICHAGE SFML
//ATTENTION SFML SUPPORTE UN NOMBRE LIMITE DE SPRITE EN FCT DU PC

/*Headers*/
/**
 * \fn void maj_angle(int vecX, int vecY, int rayon, double* angle)
 * \brief Met à jour \a angle selon la distance entre le centre de la caméra et la cible, avec un tolérance circulaire définie par rayon
 * \author Jacques
 * \param vecX composante X de la cible par rapport au centre de l'image
 * \param vecY composante Y de la cible par rapport au centre de l'image
 * \param rayon tolérance avant changement d'angle
 * \param angle tableau contenant les deux angles à modifier
 */
void maj_angle(int vecX, int vecY, int rayon, double* angle); //Met à jour l'angle selon la distance CentreCamera - Cible  

/**
 * \fn int ajust_pos(int pos, int ref) 
 * \brief permet d'éviter des positions supérieures à ref considérées comme aberrantes.
 * \details retourne 0 si la position est supérieure à ref.
 * \author Jacques
 * \param pos position à tester
 * \param ref position de référence
 * \return renvoie la position corrigée
 */
int ajust_pos(int pos, int ref); 

/**
 * \fn void controle_moteur(double* angle)
 * \brief Fonction d'envoie des angles aux moteurs
 * \details 
 * \author Jacques
 * \param angle tableau des angles moteurs
 */
void controle_moteur(double* angle);//Envoie les angles au moteur 

/**
 * \fn int limite_moteur(int val_pwm)
 * \brief Fonction qui vérifie que les valeurs envoyees aux moteurs sont correctes
 * \details Valeur minimale = 30, valeur maximale = 130 (déterminées expérimentalement)
 * \author Jacques
 * \param val_pwm valeur pwm dont on veut vérifier la valeur
 * \return renvoie l'angle corrigée
 */
int limite_moteur(int val_pwm);//Verifie que les valeurs envoyees aux moteurs sont correctes

/**
 * \fn void config(int* LowH, int* HighH, int* LowS, int* HighS, int* LowV, int* HighV)
 * \brief Fonction d'affichage du panneau de configuration de la \a couleur à suivre
 * \author Antoine
 * \details Panneau de configuration comprenant 6 slidebarres pour configurer la couleur suivie en HSV. La fonction modifie les valeurs limites LowH/   
 * HighH de la plage HUE, LowS/HighS de la plage SATURATION, LowV/HighV de la plage VALUE.

 * \param LowH Valeur \a basse de la plage de HUE suivie. 
 * \param HighH Valeur \a haute de la plage de HUE suivie.
 * \param LowS Valeur \a basse de la plage de SATURATION suivie.
 * \param HighS Valeur \a haute de la plage de SATURATION suivie.
 * \param LowV Valeur \a basse de la plage de VALUE suivie.
 * \param HighV Valeur \a haute de la plage de VALUE suivie.
 */
void config(int* LowH, int* HighH, int* LowS, int* HighS, int* LowV, int* HighV); //Affiche le panneau de configuration de tracking avec les arguments comme valeur de base

/**
 * \fn void affichage_config(IplImage* frame, IplImage* HSV, IplImage* Binaire);
 * \brief Fonction d'affichage du flux vidéo, du flux en HSV et de sa binarisation
 * \author Antoine
 * \param frame image contenant la frame capturé par la caméra
 * \param HSV image contenant l'image passé en HSV
 * \param Binaire image contenant l'image binarisée
 */
void affichage_config(IplImage* frame, IplImage* HSV, IplImage* Binaire); //Affiche le flux vidéos et ses différent traitements

/**
 * \fn void Affichage_Tracking(IplImage* frame, int posX, int posY, int width, int height)
 * \brief Fonction d'affichage des informations de suivi
 * \details Dessine sur \a frame la zone de tolérance de suivie au centre de l'image et le curseur de position de l'objet ayant pour coordonées \a posX et \a posY

 * \author Antoine
 * \param frame image a modifier
 * \param posX composante X du curseur a dessiner
 * \param posY composante Y du curseur a dessiner
 * \param width largeur (en \a pixel) de \a frame
 * \param height hauteur (en \a pixel) de \a frame
 */
void Affichage_Tracking(IplImage* frame, int posX, int posY, int width, int height); //Dessine les informations de tracking sur frame

/**
 * \fn void Position_moy(IplImage* Binaire, int* posX, int * posY)
 * \brief Effectue le baricentre des pixels d'une image binaire pour obtenir la postion de l'objet
 * \author Antoine
 * \param Binaire image binarisée (matrice de 0 ou de 1)
 * \param posX contient la composante X du barycentre
 * \param posY contient la composante Y du barycentre
 */
void Position_moy(IplImage* Binaire, int* posX, int * posY); //Effectue le baricentre des pixels d'une image binaire pour obtenir la postion de l'objet

/**
 * \fn void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV)
 * \brief Effectue une binarisation de \a frame en fonction des bornes \a HSV
 * \details Passe la frame en HSV puis binarise l'image en fonction des bornes LowH, HighH, LowS, HighS, LowV, HighV
 * \author Antoine
 * \param frame image contenant la frame capturé par la caméra
 * \param HSV image contenant l'image passé en HSV au terme de la fonction
 * \param Binaire image contenant l'image binarisée au terme de la fonction
 * \param LowH Valeur \a basse de la plage de HUE suivie. 
 * \param HighH Valeur \a haute de la plage de HUE suivie.
 * \param LowS Valeur \a basse de la plage de SATURATION suivie.
 * \param HighS Valeur \a haute de la plage de SATURATION suivie.
 * \param LowV Valeur \a basse de la plage de VALUE suivie.
 * \param HighV Valeur \a haute de la plage de VALUE suivie.
 */
void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV); //Effectue une binarisation de frame en fonction des bornes HSV

/**
 * \fn int main(int argc, char* argv[])
 * \brief Entrée du programme
 * \author Antoine / Jacques
 * \return EXIT_SUCCESS : Arrêt normal du programme, EXIT_FAILURE : Le programme a rencontrée une erreur au cours de son execution
 */
int main(int argc, char* argv[])
{
	//Initialisations 
	int height,width;  //parameters of the image we are working on
	int posX, posY; //Position objet
	int boucle;

	double angle[2] = {100,100};

	int tracking; //0 = tracking OFF


#ifdef SFML
	//Initialisation SFML

	sf::Texture txFlux;
	sf::Sprite spFlux;
	sf::Image imFlux;
  	sf::Event event;

	tracking = 0; //Pas de tracking de base en mode SFML
	
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
      
     // capture size - 
    CvSize size = cvSize(width,height);
  

#ifdef SFML
	//Création de la fenetre principale
	sf::RenderWindow window(sf::VideoMode(width+300, height), "KirbyTrack");
#endif

    // Initialize different images that are going to be used in the program
    IplImage*  hsv_frame    = cvCreateImage(size, IPL_DEPTH_8U, 3); // image converted to HSV plane
    IplImage*  threshold   = cvCreateImage(size, IPL_DEPTH_8U, 1);

    
  //Controle couleur
#ifdef KIRBY
	//Setup Kirby
	int iLowH = 152;
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
	tracking = 1; //Tracking de base en mode CONFIG
#endif
	 
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
        window.clear(sf::Color::White);	

//Affichage de la frame

	//Le chargement pourrait etre plus optimisé en créant nous me l'image SFML en parcourant l'IplImage

	//Enregistrement de la frame openCV
	cvSaveImage("Stock SFML/temp.jpg", frame);
	
	//Chargement de la frame en texture SFML
	if (!txFlux.loadFromFile("Stock SFML/temp.jpg")){
		printf("Erreur chargement image SFML\n" );
                break;
	}

	spFlux.setTexture(txFlux);
	window.draw(spFlux);


	sf::Vector2i PosMouse = sf::Mouse::getPosition(window);
//Detection du bouton tracking
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)&&(PosMouse.x>640)&&(PosMouse.x<760)&&(PosMouse.y>0)&&(PosMouse.y<120)){
		//printf("\n\n\n OK \n\n\n");
		if (tracking){ tracking = 0;}
		else tracking = 1;
		cvWaitKey(100);
	}
	//printf("Pos Mouse : %d %d \n", PosMouse.x, PosMouse.y);

//Dessin du bouton de tracking
	sf::Texture txBut;
	sf::Sprite button_tracking;
	
	if (!txBut.loadFromFile("Stock SFML/button.png")){
		printf("Erreur chargement image SFML\n" );
                break;
	}
	
	button_tracking.setTexture(txBut);
	button_tracking.setScale(0.5,0.5);
	button_tracking.setPosition(sf::Vector2f(width+20, 20));

	if(tracking){ button_tracking.setColor(sf::Color::Green); }
	else{ button_tracking.setColor(sf::Color::Red); }

	window.draw(button_tracking);

//Ajout du texte
	sf::Font font;
	if (!font.loadFromFile("Stock SFML/arial.ttf")){
    		printf("Erreur chargement police SFML\n" );
                break;
	}

	sf::Text text;
	// choix de la police à utiliser
	text.setFont(font); // font est un sf::Font

	// choix de la chaîne de caractères à afficher
	text.setString("Tracking Moteur");

	// choix de la taille des caractères
	text.setCharacterSize(24); // exprimée en pixels, pas en points !

	//text.setFillColor(sf::Color::Black);
	text.setColor(sf::Color::Black);

	text.setPosition(sf::Vector2f(width+100, 35));
	
	window.draw(text);

	/* Update the window */
        window.display();

#endif
//Envoie données moteurs
	if(tracking){
		//Mouvements moteurs
		//printf("-PREMAJ_ANGLE...: %d %d\n",width,height);

		maj_angle(ajust_pos(posX-width/2,width), ajust_pos(posY-height/2,height), height*JEU, angle);
		controle_moteur(angle); 

		cvWaitKey(50);
	}


#ifdef CONFIG
		affichage_config(frame, hsv_frame, threshold); //Affichage du flux vidéo et de ses traitements
	
		if( (cvWaitKey(10) ) >= 0 ) break; //Arret capture
#endif

	}
	
	// Release the capture device housekeeping
	cvReleaseCapture( &capture );
     
	cvReleaseImage(&threshold);
	cvReleaseImage(&hsv_frame);
	cvReleaseImage(&frame);
     
	return EXIT_SUCCESS;
}


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

void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV){ //Effectue une binarisation de frame en fonction des bornes HSV

	// Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
        cvCvtColor(frame, HSV, CV_BGR2HSV);
	
	//Blur
	cvSmooth( HSV, HSV, CV_GAUSSIAN, 15, 0,0,0); //suppression des parasites par flou gaussien

	//Binarisation

	CvScalar valinf={(double)LowH,(double)LowS,(double)LowV};
	CvScalar valsup={(double)HighH,(double)HighS,(double)HighV};

        cvInRangeS(HSV, valinf,valsup, Binaire);
	
	//En cas d'erreur sur les trois ligne précédentes
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
