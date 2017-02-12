#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "main.h"

#define BUFFER_MAX_SIZE 100

static const char *couleurs[5] = {"rouge", "vert", "bleu", "blanc", "noir"};
static int HAUTEUR_FENETRE, LARGEUR_FENETRE;

int main(int argc, char* argv[]) {
    printf("Bienvenue sur Dessin. v0.1\n");
    printf("Pour accedez a la liste des commandes : /help .\n");
    printf("Pour quitter Dessin tapez /quit .\n");

    // CONNEXION
    printf("Connexion en cours ...\n");
    HAUTEUR_FENETRE = 1000;
    LARGEUR_FENETRE = 1000;
    printf("Vous etes connecte !\n\n");
    // FIN CONNEXION

    printf("Liste des couleurs disponibles : ");
    afficherCouleurs();

    printf("Liste des commandes : /help, /couleur, /ligne, /rect\n\n");

    char *input = calloc(1,1), buffer[BUFFER_MAX_SIZE];
    while(1) { // On sort de la boucle lorsque l'utilisateur tape /quit (via la foncion inputHandler)
        printf("Entrez une commande : ");
        fgets(buffer, BUFFER_MAX_SIZE, stdin); // Entrée utilisateur
        input = realloc(input, strlen(buffer)); // On définit la tailel qu'input doit avoir
        strcpy(input, buffer); // On copie le contenu de buffer dans input
        strtok(input, "\n"); // On retire le \n
        inputHandler(input);
    }
    return 1;
}

void inputHandler(char *input) {
    int i;
    for(i = 0; input[i]; i++) input[i] = tolower(input[i]); // Passage en minuscule
    if(strcmp(input, "/help") == 0) help();
    else if (strcmp(input, "/clear") == 0) clear();
    else if (strcmp(input, "/couleur") == 0) afficherCouleurs();
    else if (strncmp("/ligne", input, 6) == 0) dessinerFigure(input, 'l');
    else if (strncmp("/rect", input, 5) == 0) dessinerFigure(input, 'r');
    else if (strcmp(input, "/quit") == 0) quit();
    else printf("Commande inexistante ou invalide. Tapez /help pour avoir de l'aide.\n\n");
}

void help() {
    printf("Pour effacer la fenetre de dessin tapez : /clear\n");
    printf("Pour avoir la liste des couleurs tapez : /couleur\n");
    printf("Pour tracer une ligne : /ligne <x1> <y1> <x2> <y2> <couleur>\n");
    printf("Pour tracer un rectangle : /rect <x> <y> <largeur> <hauteur> <couleur>\n");
    printf("\n");
}

void clear() {
    printf("Nettoyage de la fenetre !\n");
    printf("\n");
}

void afficherCouleurs() {
    int i;
    for(i = 0; i < sizeof(couleurs) / sizeof (*couleurs) ; i++)
        printf("%s, ", couleurs[i]);
    printf("\n");
    printf("\n");
}

void dessinerFigure(char *input, char typeFigure) {
    int i = 0;
    char *arg[6], *token = strtok(input, " ");
    while(token != NULL) {
        arg[i] = malloc(strlen(token) + 1);
        strcpy(arg[i], token);
        token = strtok(NULL, " ");
        i++;
    }

    if(i != 6) {
        printf("Vous n'avez pas entrez suffisament de parametres.\nTapez /help pour avoir de l'aide.\n");
        return;
    }
    int x1 = atoi(arg[1]), y1 = atoi(arg[2]), x2 = atoi(arg[3]), y2 = atoi(arg[4]);
    char *couleur = arg[5];
    for(i = 0; couleur[i]; i++) couleur[i] = tolower(couleur[i]); // Passage en minuscule

    if(typeFigure == 'l') dessinerLigne(x1, y1, x2, y2, couleur);
    else if (typeFigure == 'r') dessinerRectangle(x1, y1, x2, y2, couleur);
    else printf("Figure invalide."); // Improbable
}

void dessinerLigne(int x1, int y1, int x2, int y2, char *couleur) {
    if(x1 > -1 && x1 < LARGEUR_FENETRE && y1 > -1 && y1 < HAUTEUR_FENETRE && x2 > -1 && x2 < LARGEUR_FENETRE && y2 > -1 && y2 < HAUTEUR_FENETRE && couleurExiste(couleur) == 1) {
        // ENVOI AU SERVEUR POUR DESSINER
        printf("Ligne envoye pour affichage au serveur !\n");
        printf("\n");
    } else {
        printf("Commande invalide ! x1 et x2 doivent etre compris entre 0 et %d et y1 et 2 entre 0 et %d et la couleur doit etre dans cette liste : ", LARGEUR_FENETRE, HAUTEUR_FENETRE);
        afficherCouleurs();
    }
}

void dessinerRectangle(int x, int y, int largeur, int hauteur, char* couleur) {
    if(x > -1 && x < LARGEUR_FENETRE && y > -1 && y < HAUTEUR_FENETRE && largeur > -1 && hauteur > -1 && couleurExiste(couleur) == 1) {
        // ENVOI AU SERVEUR POUR DESSINER
        printf("Rectangle envoye pour affichage au serveur !\n");
        printf("\n");
    } else {
        printf("Commande invalide ! x doit etre compris entre 0 et %d, y entre 0 et %d et la couleur doit être dans cette liste : ", LARGEUR_FENETRE, HAUTEUR_FENETRE);
        afficherCouleurs();
    }
}

int couleurExiste(char *couleur) {
    int i;
    for(i = 0; i < sizeof(couleurs) / sizeof (*couleurs) ; i++) {
        if(strcmp(couleur, couleurs[i]) == 0)
            return 1;
    }
    return 0;
}

void quit() {
    printf("Extinction du client.\n");
    printf("\n");
    // Fermer les sockets ici ...
    exit(0);
}
