#ifndef LISTA_H
#define LISTA_H

typedef struct Nodo_{
	int valor;
	struct Nodo_ * prox;
	struct Nodo_ * ant;
}Nodo;

typedef struct Lista_{
	Nodo * primeiro;
	Nodo * ultimo;
	int n;
}Lista;

typedef struct{
	Lista * L;
	char * arquivo;
}Objeto_Thread;

Lista * Cria_Lista();

void Libera(Lista * L);

Nodo* Cria_Nodo(int x);

void Insere(Lista *L, int x);

void Mostra_Lista(Lista *L);

void remove_maior(Nodo *L);

#endif
