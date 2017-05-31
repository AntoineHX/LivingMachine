#ifndef FONCTION_H
#define FONCTION_H

/**
 * \file      fonction.h
 * \author    Jacques / Antoine
 * \date       avril - mai 2017
 * \brief      Bibliothèque, Headers et Documentation des fonctions
 *
 */


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
//#include <SFML/System.hpp> //inutilisé pour le moment

/*DEFINE*/
//ATTENTION AFFICHAGE OPENCV INCOMPATIBLE AVEC AFFICHAGE SFML
//ATTENTION SFML SUPPORTE UN NOMBRE LIMITE DE SPRITE EN FCT DU PC
#define JEU 0.15
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
 * \brief Programme en mode suivi de l'étoile (Objet jaune)
 * 
 * \def JEU
 * \brief Coefficient de tolérance pour le suivi d'objet
 *
 * \def MAX_FACE
 * \brief Nombre maximum de faces traitées
 */
//#define CONFIG
#define SFML
//#define KIRBY
#define ETOILE

#define MAX_FACE 2


/**
 * \struct face
 * \brief Contient les informations sur chaque face détectée : positions, largeur
 * \param point coordonnées en x et y du centre de la face
 * \param largeur largeur de la face
 */
/*STRUCTURE*/
typedef struct _face{
	CvPoint point; /*!< coordonnées en x et y du centre de la face */
	int largeur; /*!< largeur de la face */
} face;

typedef face* pface;



/*HEADERS*/

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
 * \bug Nécessite changement de l'utilisation de cvInRangeS sur certain PC
 */
void traitement(IplImage* frame, IplImage* HSV, IplImage* Binaire, int LowH, int HighH, int LowS, int HighS, int LowV, int HighV); //Effectue une binarisation de frame en fonction des bornes HSV

/**
 * \fn int image_CV2SFML(IplImage* imcv, sf::Image imFlux)
 * \brief Convertit une image opencv (IplImage) en une image SFML (sf::Image)
 * \author Antoine
 * \param imcv image OpenCv à convertir
 * \param imFlux image SFML convertie en sortie de la fonction
 * \return 0 si la convertion est réussie
 * \bug A debugger ! Inutilisable en l'état
 */
int image_CV2SFML(IplImage* imcv, sf::Image imFlux); //Construction de imsf (RGBA) à partir de imcv (BGR), avec alpha constant (=1)

/**
 * \fn CvHaarClassifierCascade* init_cascade()
 * \brief Charge les fichiers cascades pour la reconnaissance faciale
 * \details fichier(s) chargé(s) : "haarcascade_frontalface_alt.xml"
 * \author Jacques	
 * \return Renvoie la cascade
 */
CvHaarClassifierCascade* init_cascade();

/**
 * \fn void detect_and_draw( IplImage* img, CvHaarClassifierCascade* cascade, face** tab_face )
 * \brief Détecte et renvoie un rectangle pour chaque visage sur l'image
 * \author Jacques
 * \param img Image sur laquelle on veut détecter des visages
 * \param cascade structure contenant les données pour alimenter l'algorithme de reconnaissance d'image
 * \param tab_face tableau des faces détectées
 */
void detect_and_draw( IplImage* img, CvHaarClassifierCascade* cascade, face** tab_face );

/**
 * \fn int* get_color(IplImage* image, face* rec_face)
 * \brief retourne la couleur moyenne des pixels dans rec_face
 * \author Antoine / Jacques
 * \param image image d'où on cherche la moyenne
 * \param imFlux image SFML convertie en sortie de la fonction
 * \return 0 si la convertion est réussie
 */
void get_color(IplImage* image, face* rec_face, int* BGR);

#endif
