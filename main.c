#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "lista.h"
#include "funcoes.h"

#define size_int 4
#define size_file 1024

#ifdef __unix__
#define SO_ 1
#elif defined(_WIN32) || defined(WIN32)
#define SO_ 2
#endif

extern unsigned long long tam_output;

//variaveis
extern pthread_mutex_t count_mutex;
extern int quant_nucleos;

int main(){
	srand(time(NULL));
	if(SO_ == 1){
		retorna_nucleos_unix(); 
	}else{
		retorna_nucleos_dos();
	}
	
	Lista * L=Cria_Lista();
	
	carrega_lista(L);
	Lista * Vetor=particiona_lista(L);
	
	ordena_lista(Vetor);

	cria_saida(Vetor);

	int es_ordenada=esta_ordenado();
	if(es_ordenada == 0)
		printf("Lista Totalmente Ordenada!!! :D \n");
	else
		printf("Lista Não-Ordenada Completamente!!! %d Valores Desordenados :( \n",es_ordenada);
	
	return 0;
}
