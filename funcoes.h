#ifndef FUNCOES_H
#define FUNCOES_H

#include "lista.h"

int quant_nucleos;
unsigned long long tam_output;

char atoi_reverse(int n);
void retorna_nucleos_unix();
void retorna_nucleos_dos();
void * carrega_lista_threads(void * obj);
void carrega_lista(Lista *L);
void cria_saida(Lista * L);
Lista * particiona_lista(Lista *L);
void ordena_lista(Lista *Vetor);
void entrelaca_listas(Lista * Vetor);
void swap(int *a, int *b);
#endif
