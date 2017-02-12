/**
 * Fonction qui selon l'entrée utilisateur déclenche des actions
 * @param input l'entrée de l'utilisateur sans le retour à la ligne
 */
void inputHandler(char *input);

/**
 * Fonction qui affiche l'aide pour l'utilisateur
 */
void help();

/**
 * Fonction qui initialise le socket et une adresse qui correspond au serveur.
 */
void udpInit();

/**
 * Foncton qui souhaite la bienvenue à l'utilisateur et lui affiche les options disponibles
 */
void msgLancement();

/**
 * Boucle principale du programme, recupère les input de l'utilisateur et lance un traitement.
 */
void mainLoop();

/**
 * Fonction qui lance perror avec un message et arrête le programme
 * @param msg le message que perror va utilisé
 */
void error(char *msg);

/**
 * Fonction qui demande au serveur de nettoyer sa fenetre
 */
void clear();

/**
 * Fonction qui affiche la liste des couleurs disponibles
 */
void afficherCouleurs();

/**
 * Fonction qui va envoyer un serveur une figure a dessiner
 * @param input la commande de l'utilisateur
 * @param le type de figure à dessiner
 */
void dessinerFigure(char *input, char typeFigure);

/**
 * Fonction qui regarde si une chaine passé en paramètre
 * est présente dans le tableau des couleurs disponibles
 * @param couleur la chaine en question
 * @return 1 si la chaine est dans le tableau des couleurs, 0 sinon
 */
int couleurExiste(char *couleur);

/**
 * Fonction qui va terminer le programme proprement
 * en fermant la socket.
 */
void quit();

/**
 * Fonction qui envoi au serveur une image
 * @param input une chaine de type /img nomImage.extension
 */
void envoyerImage(char *input);

/**
 * Fonction qui initialise une socket (ipv4, tcp) et une adresse (celle du serveur)
 * @param sock la socket à init
 * @param adr l'adresse à init
 * @return 1 si réussit, 0 sinon
 */
int initTcp(int *sock, struct sockaddr_in *adr);

/**
 * Fonction qui analyse une chaine et renvoi si c'est un nombre ou non
 * @param str le chaine en question
 * @return 1 si c'est un nombre, 0 si non
 */
int isDigit(char *str);
