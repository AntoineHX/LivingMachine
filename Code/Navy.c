#include "fonction.h"


int main(int argc, char* argv[])
{
	//Initialisations
	int height,width;  //parameters of the image we are working on
	int posX, posY; //Position objet
	int boucle;
	double angle[2] = {100,100};
	int tracking;

  //Ouverture flux camera
  CvCapture* capture = cvCaptureFromCAM( 0 );
    
  if( !capture ){
  	printf("ERROR: capture is NULL \n" );
  	exit(EXIT_FAILURE);
  } 
    
  //Initialisation frame
  IplImage* frame = cvQueryFrame( capture );
  height = frame->height;
  width = frame->width;
  CvSize size = cvSize(width,height);
  IplImage*  hsv_frame    = cvCreateImage(size, IPL_DEPTH_8U, 3); // image converted to HSV plane
  IplImage*  threshold   = cvCreateImage(size, IPL_DEPTH_8U, 1);  

	//Initialisation detection visage
	CvHaarClassifierCascade* cascade = init_cascade();

#ifdef SFML
	//Initialisation SFML
	sf::Texture txFlux;
	sf::Sprite spFlux;
	sf::Image imFlux;
  sf::Event event;

	tracking = 0; //Pas de tracking de base en mode SFML
	//Création de la fenetre principale
	sf::RenderWindow window(sf::VideoMode(width+300, height), "KirbyTrack");
#endif
    
  //Controle couleur
#ifdef KIRBY
	int tab_HSV[6] = {152,179,48,255,101,255};
#endif

#ifdef ETOILE
	int tab_HSV[6] = {20,30,100,255,100,255};
#endif	

#ifdef CONFIG
	//Affichage du panneau de config
	config(tab_HSV,tab_HSV+1,tab_HSV+2,tab_HSV+3,tab_HSV+4,tab_HSV+5);

	boucle = 1;
	tracking = 0; //Tracking de base en mode CONFIG
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

		detect_and_draw(frame,cascade);

#ifdef SFML
	//Affichage SFML
	/* Clear the screen */
        window.clear(sf::Color::White);	

	//Conversion de la frame en image smfl
	/*if(image_CV2SFML(frame, imFlux)){
		printf("Erreur conversion OpenCV-SFML\n");
		break;
	}
     	*/

	//Enregistrement de la frame openCV
	cvSaveImage("Stock SFML/temp.jpg", frame);
	
	//Chargement de la frame en texture SFML
	if (!txFlux.loadFromFile("Stock SFML/temp.jpg")){
		printf("Erreur chargement image SFML\n" );
                break;
	}

	spFlux.setTexture(txFlux);

	window.draw(spFlux);

	//TEST SFML
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

/*
	//Dessin du bouton reset
	sf::Texture txBut2;
	sf::Sprite button_reset;

	if (!txBut2.loadFromFile("Stock SFML/button.png")){
		printf("Erreur chargement image SFML\n" );
                break;
	}
	
	button_reset.setTexture(txBut2);
	button_reset.setScale(0.5,0.5);
	button_reset.setPosition(sf::Vector2f(width+20, 60));

	window.draw(button_reset);
*/
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
/*
	//Link
	sf::Texture txLink;
	sf::Sprite Link;
	
	if (!txLink.loadFromFile("Stock SFML/link.png")){
		printf("Erreur chargement image SFML\n" );
                break;
	}
	
	Link.setTexture(txLink);
	Link.setPosition(sf::Vector2f(posX-75, posY-75));

	window.draw(Link);
*/
	/* Update the window */
        window.display();
#endif


	if(tracking){
		//Mouvements moteurs
		//printf("-PREMAJ_ANGLE...: %d %d\n",width,height);

		maj_angle(ajust_pos(posX-width/2,width), ajust_pos(posY-height/2,height), height*JEU, angle);
		controle_moteur(angle); 

		cvWaitKey(50);
	}


#ifdef CONFIG
		/*
		affichage_config(frame, hsv_frame, threshold); //Affichage du flux vidéo et de ses traitements
		*/
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
