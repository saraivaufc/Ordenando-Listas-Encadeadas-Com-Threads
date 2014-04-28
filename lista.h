#include <stdio.h>
#include <stdlib.h>

typedef struct node_{
	int valor;
	struct node_ * prox;
	struct node_ * ant;
}node;
typedef struct{
	int tam;
	node * primeiro;
	node * ultimo;
}Lista;

Lista *cria_lista(){
	Lista *nova=(Lista *) malloc(sizeof(Lista));
	nova->tam=0;
	nova->primeiro=NULL;
	nova->ultimo=NULL;
	return nova;
}

void inserir_vazia(Lista *L, int valor){
	node * novo=(node *) malloc(sizeof (node));
	novo->valor=valor;
	novo->ant=L->primeiro;
	novo->prox=L->ultimo;
	L->primeiro=novo;
	L->ultimo=novo;
	L->tam++;
}



void insere_comeco(Lista * L , int valor){
	if(L->tam == 0){
		inserir_vazia(L,valor);
		return;
	}
	node * novo=(node *) malloc(sizeof(node));
	novo->valor=valor;
	novo->ant=NULL;
	novo->prox=L->primeiro;
	L->primeiro->ant=novo;
	L->primeiro=novo;
	L->tam++;
	return;
}
void insere_fim(Lista * L, int valor){
	node * novo=(node *) malloc(sizeof(node));
	novo->valor=valor;
	novo->prox=NULL;
	node *aux;
	for(aux=L->primeiro ; aux->prox != NULL ; aux=aux->prox){};
	aux->prox=novo;
	novo->ant=aux;
	L->tam++;
	return;
}

void mostra_lista(Lista *L){
	for(node *aux=L->primeiro ; aux != NULL ; aux=aux->prox){
		printf("[%d]",aux->valor);
	}
	return;
}

void remove_lista(Lista *L, int valor){
	for(node * aux= L->primeiro ; aux != NULL ; aux=aux->prox){
		if(aux->valor == valor){
			//verifica se está no começo
			if(aux->ant == NULL){
				L->primeiro=aux->prox;
				aux->prox->ant=NULL;
				free(aux);
				L->tam--;
				return;
			}
			//verifica se está no fim
			if(aux->prox == NULL){
				aux->ant->prox=NULL;
				L->tam--;
				free(aux);
				return;
			}
			aux->ant->prox=aux->prox;
			aux->prox->ant=aux->ant;
			L->tam--;
			free(aux);
			return;
		}
	}
}
