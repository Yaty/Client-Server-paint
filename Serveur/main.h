#include "display.h"
#include "utils.h"

/**
 * Fonction qui dessine tout les rectangles enregistrés
 */
void dessinerRectangles();

/**
 * Fonction qui dessine toutes les lignes enregistrées
 */
void dessinerLignes();

/**
 * Fonction permettant de gérer les commandes des clients
 * @param msg la commande du client
 * @param adr les infos du client demandeur
 * @param sock le socket udp pour parler au client
 */
void gererCommandeClient(char *msg, struct sockaddr_in adr, int sock);

/**
 * Fonction pour ajouter un rectangle au dessin
 * @param commande la commande, elle contient les coordonnées x, y, la largeur, la hateur et la couleur du rectangle
 */
void ajouterRectangleDessin(char **commande);

/**
 * Fonction pour ajouter une ligne au dessin
 * @param commande la commande, elle contient les coordonnées x1, y1, x2, y2 et la couleur de la ligne
 */
void ajouterLigneDessin(char **commande);

/**
 * Fonction qui retourne une structure Argb correspondant à la couleur passé en paramètre
 * @param une couleur (rouge, vert ...) en chaine de caractère
 * @return la couleur correspondante via la structure Argb
 */
Argb strToArgb(char *couleurStr);

/**
 * Fonction qui rend la fenetre blanche
 */
void nettoyerFenetre();

/**
 * Fonction qui analyse une chaine et renvoi si c'est un nombre ou non
 * @param str le chaine en question
 * @return 1 si c'est un nombre, 0 si non
 */
int isDigit(char *str);

/**
 * Fonction qui initilise une socket, une adresse, et réalise le bind
 */
void initUdp();

/**
 * Fonction principale, affiche via SDL et recupère les commandes utilisateurs via le réseau
 */
void mainLoop();

/**
 * Wrapper de perror, ferme le programme en plus
 */
void error(char *msg);

/**
 * Fonction qui va gérer les images (protocol, affichage ...)
 * @param adrClient la structure representant le client qui envoi l'image
 * @param sockClient le socket udp pour envoyer un réponse au client
 */
void gererImage(struct sockaddr_in adrClient, int sockClient);


