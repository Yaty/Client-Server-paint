all: dessinClientv0.1 dessinServeurv0.1

dessinClientv0.1: Client/main.c
	gcc -g -Wall Client/main.c -o dessinClientv0.1

dessinServeurv0.1: Serveur/display.c Serveur/main.c
	gcc -g -Wall Serveur/display.c Serveur/main.c -o dessinServeurv0.1 -lSDL2

clean:
	rm dessinClientv0.1 dessinServeurv0.1


