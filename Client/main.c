#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "main.h"

#define BUFFER_MAX_SIZE 100
#define HAUTEUR_FENETRE 600
#define LARGEUR_FENETRE 800

static const char *couleurs[5] = {"rouge", "vert", "bleu", "blanc", "noir"};
static int sock = 0;
static struct sockaddr_in adr;

/**
 * Fonction principale qui recupère les entrées utilisateurs
 * et les envoie au serveur.
 */
int main() {
    udpInit();
	msgLancement();
	mainLoop();
    quit();
    return 1;
}

/**
 * Fonction qui initialise le socket et une adresse qui correspond au serveur.
 */
void udpInit() {
    sock = socket(AF_INET, SOCK_DGRAM, 0); // ipv4, udp
    if(sock < 0) error("socket");
    adr.sin_family = AF_INET;
    adr.sin_port = htons(2017);
    adr.sin_addr.s_addr = inet_addr("127.0.0.1");
}

/**
 * Foncton qui souhaite la bienvenue à l'utilisateur et lui affiche les options disponibles
 */
void msgLancement() {
    printf("Bienvenue sur Dessin. v0.1\n");
    printf("Pour accedez a la liste des commandes : /help \n");
    printf("Pour quitter Dessin tapez /quit\n");
    printf("Liste des commandes : /help, /couleur, /ligne, /rect, /img\n");
    printf("Liste des couleurs disponibles : ");
    afficherCouleurs();
}

/**
 * Boucle principale du programme, recupère les input de l'utilisateur et lance un traitement.
 */
void mainLoop() {
    char *input = calloc(1,1), buffer[BUFFER_MAX_SIZE];
    while(1) { // On sort de la boucle lorsque l'utilisateur tape /quit (via la foncion inputHandler)
        printf("Entrez une commande : ");
        fgets(buffer, BUFFER_MAX_SIZE, stdin); // Entrée utilisateur
        input = realloc(input, strlen(buffer)); // On définit la taille qu'input doit avoir
        strcpy(input, buffer); // On copie le contenu de buffer dans input
        strtok(input, "\n"); // On retire le \n
        inputHandler(input);
    }
}

/**
 * Fonction qui selon l'entrée utilisateur déclenche des actions
 * @param input l'entrée de l'utilisateur sans le retour à la ligne
 */
void inputHandler(char *input) {
    int i;
    for(i = 0; input[i]; i++) input[i] = tolower(input[i]); // Passage en minuscule
    if(strcmp(input, "/help") == 0) help();
    else if (strcmp(input, "/clear") == 0) clear();
    else if (strcmp(input, "/couleur") == 0) afficherCouleurs();
    else if (strncmp("/ligne", input, 6) == 0) dessinerFigure(input, 'l');
    else if (strncmp("/rect", input, 5) == 0) dessinerFigure(input, 'r');
	else if (strncmp("/img", input, 4) == 0) envoyerImage(input);
    else if (strcmp(input, "/quit") == 0) quit();
    else printf("Commande inexistante ou invalide. Tapez /help pour avoir de l'aide.\n\n");
}

/**
 * Fonction qui affiche l'aide pour l'utilisateur
 */
void help() {
    printf("Pour effacer la fenetre de dessin tapez : /clear\n");
    printf("Pour avoir la liste des couleurs tapez : /couleur\n");
    printf("Pour tracer une ligne : /ligne <x1> <y1> <x2> <y2> <couleur>\n");
    printf("Pour tracer un rectangle : /rect <x> <y> <largeur> <hauteur> <couleur>\n");
	printf("Pour afficher une image : /img <nomDuFichier>\n");
    printf("\n");
}

/**
 * Fonction qui demande au serveur de nettoyer sa fenetre
 */
void clear() {
	if(sendto(sock, "C", sizeof(char), 0, (struct sockaddr*) &adr, sizeof(adr)) != sizeof(char)) perror("sendto clear");
}

/**
 * Fonction qui affiche la liste des couleurs disponibles
 */
void afficherCouleurs() {
    int i;
    for(i = 0; i < sizeof(couleurs) / sizeof (*couleurs) ; i++)
        printf("%s, ", couleurs[i]);
    printf("\n\n");
}

/**
 * Fonction qui va envoyer un serveur une figure a dessiner
 * @param input la commande de l'utilisateur
 * @param le type de figure à dessiner
 */
void dessinerFigure(char *input, char typeFigure) {
    int i = 0;
    char *arg[6], *token = strtok(input, " "); // input va ressembler à : /uneCmd param param param param param, on va la découper et ranger le tout dans un tableau
    while(token != NULL) {
        arg[i] = malloc(strlen(token) + 1);
        strcpy(arg[i], token);
        token = strtok(NULL, " "); // On découpe
        i++;
    }

    if(i != 6) {
        printf("Vous n'avez pas entrez suffisament de parametres.\nTapez /help pour avoir de l'aide.\n\n");
        return;
    }
    
    int x1 = atoi(arg[1]), y1 = atoi(arg[2]), x2 = atoi(arg[3]), y2 = atoi(arg[4]);
    char *couleur = arg[5];
    for(i = 0; couleur[i]; i++) couleur[i] = tolower(couleur[i]); // Passage en minuscule

	if(couleurExiste(couleur) == 1) {
		char buffer[sizeof(int)*4 + sizeof(couleur)];
		// On prépare une chaine selon le protocol définit (L ou R, les paramètres et la couleur et le tout entre des séparateurs (ici #))
		if(typeFigure == 'l') 
			snprintf(buffer, sizeof(buffer), "%c%c%d%c%d%c%d%c%d%c%s", 'L', '#', x1, '#', y1, '#', x2, '#', y2, '#', couleur); 	  
		else if (typeFigure == 'r') 
			snprintf(buffer, sizeof(buffer), "%c%c%d%c%d%c%d%c%d%c%s", 'R', '#', x1, '#', y1, '#', x2, '#', y2, '#', couleur); // Ici x2 = largeur et y2 = hauteur !	
		else {
			printf("Type de figure invalide. Tapez /help.\n\n");
			return;
		}
		// Puis on envoi au serveur
		if(sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*) &adr, sizeof(adr)) != strlen(buffer)) perror("sendto figure");
    } else {
		printf("Couleur inexistante, voici les couleurs disponiles : ");
		afficherCouleurs();
    }
}

/**
 * Fonction qui regarde si une chaine passé en paramètre
 * est présente dans le tableau des couleurs disponibles
 * @param couleur la chaine en question
 * @return 1 si la chaine est dans le tableau des couleurs, 0 sinon
 */
int couleurExiste(char *couleur) {
    int i;
    for(i = 0; i < sizeof(couleurs) / sizeof (*couleurs) ; i++) {
        if(strcmp(couleur, couleurs[i]) == 0)
            return 1;
    }
    return 0;
}

/**
 * Fonction qui va terminer le programme proprement
 * en fermant la socket.
 */
void quit() {
    printf("Extinction du client.\n");
    printf("\n");
    close(sock);
    exit(0);
}

/**
 * Fonction qui lance perror avec un message et arrête le programme
 * @param msg le message que perror va utilisé
 */
void error(char *msg) {
	perror(msg);
	if(sock != 0) close(sock);
	exit(-1);
}

/**
 * Fonction qui envoi au serveur une image
 * @param input une chaine de type /img nomImage.extension
 */
void envoyerImage(char *input) {
	char typeImg[5]; // extension
	memcpy(typeImg, &input[strlen(input)-4], 4);
	typeImg[4] = '\0';

	char nomImg[20];
	memcpy(nomImg, &input[5], strlen(input));
	nomImg[strlen(nomImg)] = '\0';

	if(strcmp(typeImg, ".png") == 0 || strcmp(typeImg, ".jpg") == 0 || strcmp(typeImg, "jpeg") == 0) {
		int fichier = open(nomImg, O_RDONLY); // ouverture de l'image
		if(fichier < 0) {
			perror("open");
			return;
		}

		struct stat sb;
		if(fstat(fichier, &sb) == -1) {
			perror("fstat");
			return;
		}
		long tailleFichier= sb.st_size;

		char file[tailleFichier];
		if(read(fichier, file, tailleFichier) < 0) {
			perror("read");
			return;
		}

		printf("Vous souhaitez envoyer une image au format %s de taille %ld.\n", typeImg, tailleFichier);
		printf("Entrez les coordonnees du point en haut à gauche de l'image :\n");

    	char *x = calloc(1,1), *y = calloc(1,1), buffer[BUFFER_MAX_SIZE];
        printf("Entrez X : ");
        fgets(buffer, BUFFER_MAX_SIZE, stdin); // Entrée utilisateur
        x = realloc(x, strlen(buffer)); // On définit la taille qu'input doit avoir
        strcpy(x, buffer); // On copie le contenu de buffer dans input
        strtok(x, "\n"); // On retire le \n
		memset(buffer, 0, sizeof(buffer));
	
		printf("Entrez Y : ");
        fgets(buffer, BUFFER_MAX_SIZE, stdin);
        y = realloc(y, strlen(buffer));
        strcpy(y, buffer);
        strtok(y, "\n");

		int coord[2] = {x[0], y[0]};
		
		if(sendto(sock, "I", sizeof(char), 0, (struct sockaddr*) &adr, sizeof(adr)) != sizeof(char)) { // Demande de port tcp
			perror("sendto demande au serveur d'ouvrir un port tcp : ");
			return;	
		}
		
		// Si on est là on a demandé au serveur d'ouvrir un port TCP.
		char tmp[10];
		unsigned int taille = sizeof(adr);
		int accuseReception = recvfrom(sock, tmp, sizeof(tmp)-1, 0, (struct sockaddr*) &adr, &taille);
		if(accuseReception < 0) {
			perror("recvfrom");
			return;
		}

		if(tolower(tmp[0]) == 'o' && tolower(tmp[1]) == 'k' && isDigit(x) == 1 && isDigit(y) == 1) {
			// Alors un port TCP est ouvert, on va lancer la communication
			int sock;
			struct sockaddr_in adr;			
			if(initTcp(&sock, &adr) == 1) {
				if(write(sock, coord, sizeof(coord)) == -1) { // Envoi coord
					printf("Erreur écriture coord.\n");
					close(sock);
					return;
				}

				if(write(sock, typeImg, strlen(typeImg)) == -1) { // Envoi extension
					printf("Erreur écriture extension.\n");
					close(sock);
					return;
				}

				if(write(sock, file, tailleFichier) == -1) { // Envoi pixels
					printf("Erreur lors de l'envoi du fichier\n");
					close(sock);
					return;
				}
				close(sock);
			} else {
				printf("Erreur durant l'init TCP.\n");
				return;
			}
		} else {
			printf("Vous n'avez pas entre des nombres ou le serveur n'est pas pret pour l'envoi de l'image.\n\n");
			return;
		}
	} else {
		printf("Type d'image non géré. PNG, JPG/JPEG uniquement.\n\n");
		return;
	}
}

/**
 * Fonction qui initialise une socket (ipv4, tcp) et une adresse (celle du serveur)
 * @param sock la socket à init
 * @param adr l'adresse à init
 * @return 1 si réussit, 0 sinon
 */
int initTcp(int *sock, struct sockaddr_in *adr) {
	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if(*sock < 0) {
		perror("socketTcp");
		return 0;
	}
	
	(*adr).sin_family = AF_INET;
	(*adr).sin_port = htons(2018);
	(*adr).sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(*sock, (const struct sockaddr*) &(*adr), sizeof(*adr)) < 0) {
		perror("connect");
		return 0;
	}
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
