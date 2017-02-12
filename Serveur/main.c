#include "display.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>

#elif defined (linux)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else /* sinon vous êtes sur une plateforme non supportée */
#error not defined for this platform
#endif

#define BUF_SIZE 1024
#define PORT 2017

#define NB_RECTANGLES 100
#define NB_LIGNES 100
#define HAUTEUR 600
#define LARGEUR 800

static int nbLignes = 0;
static int nbRect = 0;
static Rect rectangles[NB_RECTANGLES];
static Ligne lignes[NB_LIGNES];
static const Argb rouge = {.a = 255, .r = 255, .g = 0, .b = 0}, blanc = {.a = 255, .r = 255, .g = 255, .b = 255};

/** main function to test SDL features
 * @return 0 if ok
 */
int main(int argc, char* argv[]) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    SOCKADDR_IN sin = { 0 };

    if(sock == INVALID_SOCKET) {
      perror("socket()");
      exit(errno);
    }

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;

    if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
    {
      perror("bind()");
      exit(errno);
    }
    char buffer[BUF_SIZE];
    /* the index for the array */
    int actual = 0;
    int max = sock;

    fd_set rdfs;
    if (displayInit("Client", LARGEUR, HAUTEUR) == 0) {
        while (1) {
            displayDrawRect(0, 0, LARGEUR, HAUTEUR, blanc, true); // Reset fond blanc
            delay(30); // On ralentit pour moins consommer (c'est inutile d'être ultra fluide)
            dessinerLignes(lignes, nbLignes);
            dessinerRectangles(rectangles, nbRect);
            displayPersistentScreen();
        }
        displayQuit();
        return 0;
    }
    return 1;
} // main

void dessinerRectangles(Rect *rectangles, int nbRect) {
    if(nbRect != 0 && nbRect < NB_RECTANGLES) {
        int i;
        for(i = 0 ; i < nbRect ; i++)
            displayDrawRect(rectangles[i].x, rectangles[i].y, rectangles[i].largeur, rectangles[i].hauteur, rectangles[i].couleur, rectangles[i].remplit);
    }
}

void dessinerLignes(Ligne *lignes, int nbLignes) {
    if(nbLignes != 0 && nbLignes < NB_LIGNES) {
        int i;
        for(i = 0 ; i < nbLignes ; i++)
            displayDrawLine(lignes[i].x1, lignes[i].y1, lignes[i].x2, lignes[i].y2, lignes[i].couleur);
    }
}
