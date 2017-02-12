#include "utils.h"

int main() {
  int i;
  int s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s<0) perror("socket");

  struct sockaddr_in adr;
  adr.sin_family = AF_INET;
  adr.sin_port = htons(12345);
  adr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Envoi 20x 30 caractères en boucle rapide
  for (i=0; i<20; i++)
    if (sendto(s, "Hello!789012345678901234567890", 30, 0, (struct sockaddr*) &adr, sizeof(adr))<8)
      perror("sendto");

  char tmp[65536];
  unsigned int taille = sizeof(adr);
  int recu;

  sleep(1); // Laisse au serveur le temps de recevoir les messages

  if ((recu=recvfrom(s, tmp, sizeof(tmp)-1, 0, (struct sockaddr*) &adr, &taille))<0)
    perror("recvfrom");
  printAddr("Message recu", &adr);
  
  if (recu>-1)
    tmp[recu] = 0; // insère le \0 final au cas où le serveur ne l'aurait pas mi
  printf("Recu %d octets : %s\n", recu, tmp);

  close(s);
  return 0;
}
