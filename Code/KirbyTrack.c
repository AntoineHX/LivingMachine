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


#include "fonction.h"


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



