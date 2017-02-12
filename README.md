## Client-Server-paint

Little network project in C using SDL.
The server have a SDL window and clients can send commands to draw on it.
You can draw lines, rectangles and other figures :)
Just execute the make file, launch the server then clients !

## Instructions (sorry for non-french speakers) :
Il suffit de lancer la commande make dans votre invite de commandes afin de lancer la compilation.
Celle-ci va générer deux exécutables :
- dessinClientv0.1
- dessinServeurv0.1

Le serveur possède une fenêtre et affiche via SDL ce que les clients lui demandent.

Le client va vous proposer d'entrez des commandes afin de communiquer avec le serveur. Pour dessiner il existe 2 commandes :
- /ligne x1 y1 x2 y2 couleur
- /rect x y largeur hauteur couleur
Pour voir les couleurs disponibles vous pouvez taper /couleur, pour la liste des commandes tapez /help

On peut aussi envoyer une image en tapant /img terre.jpg (l'image doit se trouver dans le repertoire de l'exécutable).
L'image est bien reçu par le serveur mais il y a un problème lors de la ré-écriture de celle-ci côté serveur (je n'ai pas chercher à comprendre par manque de temps).

Au niveau du protocol :
- pour les figures de bases (rectangle et ligne), j'envoi tout simplement une chaine de caractère ressemblant à celle-ci : L#50#50#500#500#rouge (type x1 y1 x2 y2 couleur)
Ensuite le serveur l'interprète en découpant la chaine qu'il reçoit et dessine la figure via SDL.

- pour l'image cela ce passe en plusieurs étapes :
	1) Le client demande au serveur d'ouvrir un port TCP et attend un accusé
	2) le serveur ouvre un port tcp, et envoi l'accusé au client
	3) le client envoi à la suite les coordonnées du point en haut à gauche de l'image, l'extension de l'image puis les pixels
	4) le serveur reçoit ses informations dans le même ordre et stock toutes les informations
	5) le serveur écrit l'image dans un fichier img.png ou img.jpg mais l'image est corrompu
