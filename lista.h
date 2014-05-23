#ifndef LISTA_H
#define LISTA_H

#include <pthread.h>

typedef struct Nodo_{
	int valor;
	struct Nodo_ * prox;
	struct Nodo_ * ant;
}Nodo;

typedef struct Lista_{
	Nodo * primeiro;
	Nodo * ultimo;
	int n,n2;
}Lista;

typedef struct{
	Lista * L;
	char * arquivo;
	int tipo;
	pthread_mutex_t * mutex;
}Objeto_Thread;

Lista * Cria_Lista();

void Libera(Lista * L);

Nodo* Cria_Nodo(int x);

void Insere(Lista *L, int x);

void Mostra_Lista(Lista *L);

void remove_maior(Nodo *L);

#endif
