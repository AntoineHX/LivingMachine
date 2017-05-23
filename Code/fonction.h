#ifndef FONCTION_H
#define FONCTION_H


/*INCLUDE*/
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
#include <SFML/Window.hpp>
#include <SFML/System.hpp> //inutilisé pour le moment

/*DEFINE*/
//ATTENTION AFFICHAGE OPENCV INCOMPATIBLE AVEC AFFICHAGE SFML
//ATTENTION SFML SUPPORTE UN NOMBRE LIMITE DE SPRITE EN FCT DU PC
#define JEU 0.15
#define CONFIG
//#define SFML
#define KIRBY
//#define ETOILE

#define MAX_FACE 2



/*STRUCTURE*/
typedef struct _face{
	CvPoint point;
	int largeur;
} face

typedef struct pface *face;


/*HEADERS*/
void maj_angle(int vecX, int vecY, int rayon, double* angle); //Met à jour l'angle selon la distance CentreCamera - Cible
int ajust_pos(int pos, int ref);
void controle_moteur(double* angle);//Envoie les angles au moteur
int limite_moteur(int val_pwm);//Verifie que les valeurs envoyees aux moteurs sont correctes

void config(int* LowH, int* HighH, int* LowS, int* HighS, int* LowV, int* HighV); //Affiche le panneau de configuration de tracking avec les arguments comme valeur de base
void affichage_config(IplImage* frame, IplImage* HSV, IplImage* Binaire); //Affiche le flux vidéos et ses différent traitements
void Affichage_Tracking(IplImage* frame, int posX, int posY, int width, int height); //Dessine les informations de tracking sur frame

void Position_moy(IplImage* Binaire, int* posX, int * posY); //Effectue le baricentre des pixels d'une image binaire pour obtenir la postion de l'objet
void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV); //Effectue une binarisation de frame en fonction des bornes HSV

int image_CV2SFML(IplImage* imcv, sf::Image imFlux); //Construction de imsf (RGBA) à partir de imcv (BGR), avec alpha constant (=1)

CvHaarClassifierCascade* init_cascade();
void detect_and_draw( IplImage* img, CvHaarClassifierCascade* cascade );


#endif
