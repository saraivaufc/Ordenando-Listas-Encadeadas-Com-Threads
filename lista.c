#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

Lista * Cria_Lista(){
	Lista * L=(Lista *) malloc( sizeof (Lista) );
	L->primeiro = NULL;
	L->ultimo = NULL;
	L->n = 0;
	return L;
}

void Libera(Lista * L){
	while (L->primeiro != NULL) {
		Nodo * nodo = L->primeiro;
		L->primeiro = nodo->prox;
		free(nodo); 
	}
	L->n = 0;
}

Nodo* Cria_Nodo(int x) {
	Nodo * nodo = (Nodo *) malloc(sizeof(Nodo));
	nodo->valor = x;
	nodo->prox = NULL;
	nodo->ant = NULL;
	return nodo;  
}

void Insere(Lista *L, int x) {
	Nodo * novo=Cria_Nodo(x);
	
	if(L->n == 0){
		novo->ant=L->primeiro;
		novo->prox=L->primeiro;
		L->primeiro=novo;
		L->ultimo=novo;
	}else{
		novo->ant=NULL;
		novo->prox=L->primeiro;
		L->primeiro->ant=novo;
		L->primeiro=novo;
	}
	L->n++;
	return;
}

void remove_maior(Nodo *L){
	if(L == NULL)
		return;
	Nodo * maior=L;
	for(Nodo *aux=L ; aux != NULL ; aux=aux->prox){
		if(aux->valor > maior->valor)
			maior=aux;
	}
	maior->prox->ant=maior->ant;
	
}

void Mostra_Lista(Lista *L){
	Nodo* nodo = L->primeiro;
	while (nodo != NULL) {
		printf("[%d]",nodo->valor);
		nodo = nodo->prox;
	}
}
