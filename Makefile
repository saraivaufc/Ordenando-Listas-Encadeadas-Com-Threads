main: main.c lista.h
	gcc main.c lista.h -ggdb -std=c99 -o main -lpthread
