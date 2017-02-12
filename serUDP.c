#include "utils.h"

int main() {
  int s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s<0) perror("socket");

  struct sockaddr_in adr;
  adr.sin_family = AF_INET;
  adr.sin_port = htons(12345);
  adr.sin_addr.s_addr = INADDR_ANY;
  
  if (bind(s, (struct sockaddr*) &adr, sizeof(adr))<0)
    perror("bind");

  while (1) {
    char tmp[21];
    unsigned int taille = sizeof(adr);
    int recu;

    if ((recu=recvfrom(s, tmp, sizeof(tmp)-1, 0, (struct sockaddr*) &adr, &taille))<0) {
      perror("recvfrom");
      break;
    }
    printAddr("Message recu", &adr);
    if (recu>-1)
      tmp[recu] = 0; // insère le \0 final au cas où le serveur ne l'aurait pas mis
    printf("Recu %d octets : %s\n", recu, tmp);

    struct sockaddr_in cli;
    unsigned int tailleCli = sizeof(cli);
    if (getpeername(s, (struct sockaddr*)&cli,&tailleCli)<0)
      perror("getPeerName");
    else
      printAddr("getPeerName",&cli);

    // Mets en majuscules la chaîne : 32 octets entre le 'a' (97) et le 'A' (65)
    int i;
    for (i=0; i<recu; ++i)
      if ((tmp[i]>='a') && (tmp[i]<='z'))
	tmp[i] -= 32;
    
//    sleep(2);
    if (sendto(s, tmp, recu, 0, (struct sockaddr*) &adr, sizeof(adr))<recu) {
      perror("sendto");
      break;
    }
  }

  close(s);
  return 0;
}
