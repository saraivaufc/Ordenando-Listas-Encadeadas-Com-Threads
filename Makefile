main: main.c lista.h
	gcc -o main -Wall -std=c99 main.c lista.h  -lpthread
