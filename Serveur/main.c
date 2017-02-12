#include "display.h"
#include "main.h"

#define NB_RECTANGLES 100
#define NB_LIGNES 100
#define HAUTEUR 600
#define LARGEUR 800

static int nbLignes = 0;
static int nbRect = 0;
static Rect rectangles[NB_RECTANGLES];
static Ligne lignes[NB_LIGNES];

/** 
 * Fonction main pour lancer le serveur
 * et la fenetre SDL
 */
int main() {
	if (displayInit("Serveur", LARGEUR, HAUTEUR) == 0) { // Init SDL
		mainLoop();
        displayQuit();
    }
    return 1;
}

/**
 * Fonction principale, affiche via SDL et recupère les commandes utilisateurs via le réseau
 */
void mainLoop() {
	int sock = 0, recu = 0;
	struct sockaddr_in adr;
	initUdp(&sock, &adr, &recu);
	unsigned int taille = sizeof(adr);
	Argb blanc = strToArgb("blanc");
	char buffer[100];
    while (1) { // Boucle principale
    	// Zone SDL
		displayDrawRect(0, 0, LARGEUR, HAUTEUR, blanc, true); // Reset fond blanc
        dessinerLignes();
        dessinerRectangles();
        displayPersistentScreen();
        if(checkEvents() == true) break; // Vrai si bouton pour fermer la fenetre activé
        // Fin Zone SDL
		
		// Zone réseau
		if((recu = recvfrom(sock, buffer, sizeof(buffer)-1, 0, (struct sockaddr*) &adr, &taille))<0) // On met dans le buffer tmp s'il y a un msg
			// Les erreurs EAGAIN & EWOULDBLOCK se déclenchent quand on a eu un timeout (pas eu de message pendant 20ms), cela ne néccesite pas de déclencher perror
			if(errno != EAGAIN && errno != EWOULDBLOCK) perror("recvfrom");  
					
		if(recu > 0) {
			buffer[recu] = '\0';
			gererCommandeClient(buffer, adr, sock); // Va gérer la commande du client
			memset(buffer, 0, sizeof(buffer));
		}
		// Fin zone réseau
    }
	close(sock);
}

/**
 * Fonction qui initilise une socket, une adresse, et réalise le bind
 */
void initUdp(int *sock, struct sockaddr_in *adr,int *recu) {
	*sock = socket(AF_INET, SOCK_DGRAM, 0); // ipv4, udp
	if(*sock < 0) error("socket");

	// Les caractéristiques réseau du serveur
	(*adr).sin_family = AF_INET;
	(*adr).sin_port = htons(2017);
	(*adr).sin_addr.s_addr = INADDR_ANY;

	struct timeval timeout;
	timeout.tv_usec = 20;

	// Permet au prochain recvfrom dans la mainloop de n'être bloquant que 20ms (ci-dessus)
	// -> pour continuer à afficher SDL et évite de bloquer s'il y a plusieurs clients
	if(setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) error("setsockopt");

	if(bind(*sock, (struct sockaddr*) &(*adr), sizeof(*adr))<0) error("bind");
}

/**
 * Fonction qui analyse une commande est retoune si elle est valide
 * @param msg la commande
 * @return 1 si elle est valide, 0 sinon
 */
int commandeValide(char *msg) {
	if(tolower(msg[0]) == 'i' || tolower(msg[0]) == 'c') return 1; // Image, en suite on gère les commandes de dessins
	
	char cmd[strlen(msg)];
	strcpy(cmd, msg);
	char *commande[6];
	char *token = strtok(cmd, "#");
	int i = 0;
	
	// Découpage de la chaîne via le délimiteur #
	while(token != NULL) {
		commande[i] = token;
		token = strtok(NULL, "#");
		i++;	
	}
	
	// Passage en minuscule du paramètre 1 et 6 (type et couleur)
	commande[0][0] = tolower(commande[0][0]);
	for(i = 0 ; commande[5][i] ; i++) commande[5][i] = tolower(commande[5][i]);
	
	// Tests sur le commande :
	if(commande[0][0] != 'r' && commande[0][0] != 'l') return 0;
	if(isDigit(commande[1]) == 0 || isDigit(commande[2]) == 0 || isDigit(commande[3]) == 0 || isDigit(commande[4]) == 0) return 0;
	if(strcmp(commande[5], "noir") != 0 && strcmp(commande[5], "blanc") != 0 && strcmp(commande[5], "rouge") != 0 &&
	strcmp(commande[5], "bleu") != 0 && strcmp(commande[5], "vert") != 0) return 0;

	return 1;
}

/**
 * Fonction qui analyse une chaine et renvoi si c'est un nombre ou non
 * @param str le chaine en question
 * @return 1 si c'est un nombre, 0 si non
 */
int isDigit(char *str) {
	int i;
	for(i = 0 ; i < strlen(str) ; i++) {
		if(!(str[i] >= '0' && str[i] <= '9'))
			return 0;
	}
	return 1;
}



/**
 * Fonction permettant de gérer les commandes des clients
 * @param msg la commande du client
 * @param adr les infos du client demandeur
 * @param sock le socket udp pour parler au client
 */
void gererCommandeClient(char *msg, struct sockaddr_in adr, int sock) {
	// Syntaxe d'une commande : type#valeur#valeur#valeur#valeur#couleur, exemple -> l#10#10#548#485#rouge
	if(commandeValide(msg) == 0) {
		printf("Commande invalide\n\n");
		return;
	}
	
	char *commande[6]; // Contient 6 éléments -> le type, les 4 valeurs et la couleur
	char *token = strtok(msg, "#");
	int i = 0;
	
	// Découpage de la chaîne via le délimiteur #
	while(token != NULL) {
		commande[i] = token;
		token = strtok(NULL, "#");
		i++;	
	}
	
	char type = tolower(commande[0][0]);

	if(type == 'l')
		ajouterLigneDessin(commande);
	else if (type == 'r')
		ajouterRectangleDessin(commande);
	else if (type == 'c')
		nettoyerFenetre();
	else if (type == 'i')
		gererImage(adr, sock);
	else
		printf("Type de figure non géré.\n\n");
}

/**
 * Fonction qui va gérer les images (protocol, affichage ...)
 * @param adrClient la structure representant le client qui envoi l'image
 * @param sockClient le socket udp pour envoyer un réponse au client
 */
void gererImage(struct sockaddr_in adrClient, int sockClient) {
	int f = fork();
	if(f == 0) {
		int s = socket(AF_INET, SOCK_STREAM, 0);
		if(s < 0) {
			perror("socket");
			exit(-1);
		}
		struct sockaddr_in adr;
		adr.sin_family = AF_INET;
		adr.sin_port = htons(2018);
		adr.sin_addr.s_addr = inet_addr("127.0.0.1");

		if(bind(s, (struct sockaddr*) &adr, sizeof(adr)) < 0) {
			perror("bind");
			return;
		}

		if(listen(s, 1) < 0) perror("listen");

		// On va indiquer au client que l'on est pret à recevoir ses infos
		if(sendto(sockClient, "OK", sizeof(char)*2, 0, (struct sockaddr*) &adrClient, sizeof(adrClient)) != sizeof(char)*2) {
			perror("sendto ok");
			return;
		}

		unsigned int taille = sizeof(adr);
		int a = accept(s, (struct sockaddr *) &adr, &taille);
		if(a < 0) {
			perror("accept");
			close(s);
			exit(-1);
		}
		
		int coord[2];
		if(read(a, coord, sizeof(coord)) == -1) {
			perror("read coord");
			return;
		}

		int lu;
		char ext[5];
		lu = read(a, ext, sizeof(ext)); // reception de l'extension de l'image
		if(lu < 0 ) {
			perror("read ext");
			return;
		}
		
		if(strncmp(ext, "jpeg", 4) == 0) strcpy(".jpg", ext); // On remplace jpeg par .jpg

		char fichier[10] = "img";
		strcat(fichier, ext);
		fichier[strlen(fichier)] = '\0';

		int fd = open(fichier, O_CREAT | O_WRONLY, S_IRWXU); // On ouvre un fichier que l'on va remplir de pixels
		unsigned long tailleFichier;

		while(1) {
			char buffer[1024];
			int recu;

			if((recu = read(a, buffer, sizeof(buffer))) < 0 ) { // Reception des pixels
				perror("read");
				break;
			}

			if(recu == 0) break;

			if(recu > -1) tailleFichier += recu;

			write(fd, buffer, sizeof(buffer)); // Ecriture dans le fichier
		}

		close(fd);
		close(a);
		
		if(strncmp(ext, ".jpg", 4) == 0) {
			printf("Youhou on va afficher un jpg !\n");
		} else if (strncmp(ext, ".png", 4) == 0) {
			printf("Youhou on va afficher un png !\n");
		} else {
			printf("Extension non géré.\n");
			return;
		}
	} else return;
}

/**
 * Fonction qui rend la fenetre blanche
 */
void nettoyerFenetre() {
	displayDrawRect(0, 0, LARGEUR, HAUTEUR, strToArgb("blanc"), true); // Reset fond blanc
	nbLignes = nbRect = 0; // On remet à zéro les compteurs
}


/**
 * Fonction pour ajouter une ligne au dessin
 * @param commande la commande, elle contient les coordonnées x1, y1, x2, y2 et la couleur de la ligne
 */
void ajouterLigneDessin(char **commande) {
	int x1 = atoi(commande[1]), y1 = atoi(commande[2]), x2 = atoi(commande[3]), y2 = atoi(commande[4]);
	char *couleurStr = commande[5];
	Argb couleur = strToArgb(couleurStr);
	Ligne ligne = {.x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2, .couleur = couleur};
	lignes[nbLignes++] = ligne; // Ajout de la nouvelle ligne dans le tableau de lignes
}

/**
 * Fonction pour ajouter un rectangle au dessin
 * @param commande la commande, elle contient les coordonnées x, y, la largeur, la hateur et la couleur du rectangle
 */
void ajouterRectangleDessin(char **commande) {
	int x = atoi(commande[1]), y = atoi(commande[2]), largeur = atoi(commande[3]), hauteur = atoi(commande[4]);
	char *couleurStr = commande[5];
	Argb couleur = strToArgb(couleurStr);
	Rect rectangle = {.x = x, .y = y, .largeur = largeur, .hauteur = hauteur, .couleur = couleur, .remplit = false};
	rectangles[nbRect++] = rectangle; // Ajout du nouveau rectangle dans le tableau des rectangles
}

/**
 * Fonction qui retourne une structure Argb correspondant à la couleur passé en paramètre
 * @param une couleur (rouge, vert ...) en chaine de caractère
 * @return la couleur correspondante via la structure Argb
 */
Argb strToArgb(char *couleurStr) {
	Argb couleur;
	int i;
	char *couleurStrLower = strdup(couleurStr);
	for(i = 0; i < strlen(couleurStrLower); i++) couleurStrLower[i] = tolower(couleurStrLower[i]); // Passage en minuscule
	
	if(strcmp(couleurStrLower, "rouge") == 0)
		couleur = (Argb) {.a = 255, .r = 255, .g = 0, .b = 0};
	else if(strcmp(couleurStrLower, "noir") == 0)
		couleur = (Argb) {.a = 255, .r = 0, .g = 0, .b = 0};
	else if(strcmp(couleurStrLower, "vert") == 0)
		couleur = (Argb) {.a = 255, .r = 0, .g = 255, .b = 0};
	else if(strcmp(couleurStrLower, "bleu") == 0)
		couleur = (Argb) {.a = 255, .r = 0, .g = 0, .b = 255};	
	else
		couleur = (Argb) {.a = 255, .r = 255, .g = 255, .b = 255}; // blanc
	return couleur;	
}


/**
 * Fonction qui dessine tout les rectangles enregistrés
 */
void dessinerRectangles() {
    if(nbRect != 0 && nbRect < NB_RECTANGLES) {
        int i;
        for(i = 0 ; i < nbRect ; i++)
            displayDrawRect(rectangles[i].x, rectangles[i].y, rectangles[i].largeur, rectangles[i].hauteur, rectangles[i].couleur, rectangles[i].remplit);
    }
}

/**
 * Fonction qui dessine toutes les lignes enregistrées
 */
void dessinerLignes() {
    if(nbLignes != 0 && nbLignes < NB_LIGNES) {
        int i;
        for(i = 0 ; i < nbLignes ; i++)
            displayDrawLine(lignes[i].x1, lignes[i].y1, lignes[i].x2, lignes[i].y2, lignes[i].couleur);
    }
}

/**
 * Wrapper de perror, ferme le programme en plus
 */
void error(char *msg) {
	perror(msg);
	exit(-1);
}
