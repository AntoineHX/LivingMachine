/**
 * \file      Navy.c
 * \author    Jacques / Antoine
 * \date       avril - mai 2017
 * \brief      Figure Libre
 *
 * \details   Suivie d'un Kirby (Rose) ou d'une étoile (Jaune) par une caméra avec mode interface utilisateur ou configuration
 *
 * \todo Stabiliser detection faciale 
 */

#include "fonction.h"
//#include "fonction.c" //Pour le problème de référence indéfinie à la compilation


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
	int boucle = 1; //On effectuera la boucle principale au moins une fois
	int i;
	float scale_x, scale_y;
	int BGR[3];

	//Initialisation detection visage
	CvHaarClassifierCascade* cascade = init_cascade();
	face* tab_face[MAX_FACE];
	for (i=0;i<2;i++){
		tab_face[i] = (face*)malloc(sizeof(face));
	}
	int rdy[MAX_FACE];

	//Link
	sf::Texture txLink;
	sf::Sprite Link;
	
	if (!txLink.loadFromFile("Stock SFML/link.png")){
		printf("Erreur chargement image SFML\n" );
                return EXIT_FAILURE;
	}

	sf::Vector2u vec_link = txLink.getSize();

	//Ganon
	sf::Texture txGanon;
	sf::Sprite Ganon;
	
	if (!txGanon.loadFromFile("Stock SFML/ganon.png")){
		printf("Erreur chargement image SFML\n" );
                return EXIT_FAILURE;
	}

	sf::Vector2u vec_ganon = txGanon.getSize();

	//Initialisation SFML

	sf::Texture txFlux;
	sf::Sprite spFlux;
	sf::Image imFlux;
  	sf::Event event;

	//Chargement son
	sf::Music aile;
	if (!aile.openFromFile("Stock SFML/aile.ogg"))
    		return EXIT_FAILURE;
	aile.setLoop(true);
	aile.play();
	
	sf::Music hey;
	if (!hey.openFromFile("Stock SFML/hey_listen.ogg"))
    		return EXIT_FAILURE;
	sf::Music watchout;
	if (!watchout.openFromFile("Stock SFML/watchout.ogg"))
    		return EXIT_FAILURE;

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
   // CvSize size = cvSize(width,height);
  


	//Création de la fenetre principale
	sf::RenderWindow window(sf::VideoMode(width, height), "KirbyTrack");
  

    while(boucle)
    { 

		boucle = window.isOpen();

		// on inspecte tous les évènements de la fenêtre qui ont été émis depuis la précédente itération
		while (window.pollEvent(event))
		{
			// évènement "fermeture demandée" : on ferme la fenêtre
			if (event.type == sf::Event::Closed)
		              window.close();
		}

		// Get one frame
		frame = cvQueryFrame( capture );
		      
		if( !frame ){
			perror("ERROR: frame is null...");
			break;
		}

		
		detect_and_draw(frame,cascade,tab_face);


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

	Link.setTexture(txLink);
	Link.setOrigin(sf::Vector2f(vec_link.x/2,vec_link.y/2));
	Link.setPosition(sf::Vector2f(tab_face[0]->point.x, tab_face[0]->point.y));
	scale_x = tab_face[0]->largeur / (1.0*vec_link.x);
	scale_y = tab_face[0]->largeur / (1.0*vec_link.y);
	//printf("%d , %d et%lf, %lf\n",tab_face[0]->largeur,vec_link.x,scale_x,scale_y);
	Link.setScale(sf::Vector2f(scale_x, scale_y));
	
	Ganon.setTexture(txGanon);
	Ganon.setOrigin(sf::Vector2f(vec_ganon.x/2,vec_ganon.y/2));
	Ganon.setPosition(sf::Vector2f(tab_face[1]->point.x, tab_face[1]->point.y));
	scale_x = tab_face[1]->largeur / (1.0*vec_ganon.x);
	scale_y = tab_face[1]->largeur / (1.0*vec_ganon.y);
	Ganon.setScale(sf::Vector2f(scale_x, scale_y));

	window.draw(Link);
	window.draw(Ganon);

	/* Update the window */

        window.display();
	
	//Marqueurs rdy
	for (i=0 ; i<MAX_FACE ; i++){
		if(tab_face[i]->largeur==0){
			rdy[i]=1;
		}
	}

	if((tab_face[1]->largeur>0) && (watchout.getStatus()==sf::Sound::Stopped) && rdy[1]){
			watchout.play();
			rdy[1]=0;
	}
	if((tab_face[0]->largeur>0) && (hey.getStatus()==sf::Sound::Stopped) && rdy[0]){
		hey.play();
		rdy[0]=0;

		printf("a\n");
		get_color(frame, tab_face[0],BGR);
		printf("B = %d, G = %d, R = %d\n",BGR[0],BGR[1],BGR[2]);
	}



}
	// Release the capture device housekeeping
	cvReleaseCapture( &capture );
     
	cvReleaseImage(&frame);
     
	return EXIT_SUCCESS;
}
