main : main.c lista.c funcoes.c quick.h
	gcc -o main -ggdb -Wall main.c lista.c funcoes.c quick.h -std=c99 -lpthread
