#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "lista.h"
#include <semaphore.h>

#define quant_arquivos 10
#define size_int 4

#define size_file 1024

int quant_nucleos;


void retorna_nucleos_unix(){
	system("cat /proc/cpuinfo | grep processor | wc -l  >> nucleos.txt");
	FILE * nucleos;
	nucleos=fopen("nucleos.txt" , "r+");
	fscanf(nucleos,"%d",&quant_nucleos);
	fclose(nucleos);
	system("rm nucleos.txt");
}
void retorna_nucleos_dos(){

}
#ifdef __unix__


#define SO 1

#elif defined(_WIN32) || defined(WIN32)

#define SO 2

#endif

char atoi_reverse(int n){
	if(n == 0) return '0';
	if(n == 1) return '1';
	if(n == 2) return '2';
	if(n == 3) return '3';
	if(n == 4) return '4';
	if(n == 5) return '5';
	if(n == 6) return '6';
	if(n == 7) return '7';
	if(n == 8) return '8';
	if(n == 9) return '9';
	return 'x';
}

void cria_arquivos(Lista *L,char tamanho[]){
	char nome_arq[6];
	nome_arq[1]='.';
	nome_arq[2]='b';
	nome_arq[3]='i';
	nome_arq[4]='n';
	nome_arq[5]='\0';

	
	srand(time(NULL));
	for(int i=0 ; i < quant_arquivos ; i++){
		nome_arq[0]=atoi_reverse(i);
		FILE * arq=fopen(nome_arq, "wb");
		if(arq == NULL){
			printf("Error ao abrir arquivo..\n");
			return;
		}
		for(int k=0;k<((int)atoi(tamanho)*size_file)/size_int;k++)
		{
			int aleatorio=rand()%10;
			fwrite (&aleatorio, sizeof(aleatorio),1, arq);
		}
		fclose(arq);
	}
}

typedef struct{
	Lista * L;//lista onde armazenará os valores
	char * arquivo;//o path do arquivo que eu vou jogar na lista
	char * tamanho;//quantidade de mb do arquivo
}Objeto_Thread;

sem_t mutex;
int counter; /* shared variable */

void * carrega_lista(void * obj){
	printf("\nThread criada...\n");
	Objeto_Thread * Objeto=(Objeto_Thread *) obj;
	FILE * arq=fopen(Objeto->arquivo, "rb");
	if(arq == NULL){
		printf("Error ao abrir o arquivo...\n");
		return NULL;
	}
	for(int k=0;k<((int)atoi(Objeto->tamanho) *size_file)/size_int;k++)
	{
		int valor;
		fread(&valor, sizeof(valor),1, arq);

		sem_wait(&mutex);
		/* entrando na região critica*/
		insere_comeco(Objeto->L,valor);
		/* saindo da região critica*/
		sem_post(&mutex);
	}
	fclose(arq);
	return NULL;
}
void * insertionSort_list(void *arg){
	Lista *L=(Lista *) arg;
	for(node *aux=L->primeiro->prox ; aux != NULL ; aux=aux->prox){
		int valor=aux->valor;
		node *j=aux->ant;
		node *anterior;
		while(j != NULL  && valor < j->valor){
			j->prox->valor=j->valor;
			anterior=j;
			j=j->ant;
		}
		if(j == NULL)
			anterior->valor=valor;
		else
			j->prox->valor=valor;
	}
	return arg;
}

int menor_das_listas(Lista *Listas){
	int menor;
	int t;
	node ** listas=(node **) malloc(quant_nucleos * sizeof (node));
	for(int i=0;i<quant_nucleos;i++){
		listas[i]=Listas->primeiro;
	}

	for(int i=0;i<quant_nucleos;i++){
		menor=listas[0]->valor;
		t=0;
		for(int k=1;k<quant_nucleos;k++){
			if(listas[k]->valor < menor){
				menor=listas[k]->valor;
				t=k;
			}
		}
	}
	node * n=listas[t];
	listas[t]=listas[t]->prox;
	free(n);
	sleep(100);
	return menor; 
}


void ordena_lista(Lista * L, char tamanho[]){
	int pedaco=(int)L->tam/quant_nucleos;
	Lista * minhas_t=(Lista *) malloc(quant_nucleos * sizeof(Lista));
	minhas_t[0].primeiro=L->primeiro;
	for(int i=1;i<quant_nucleos;i++){
		
		node * aux=L->primeiro;
		for(int k=0;k<pedaco;k++){
			aux=aux->prox;
		}
		//peraaaaaa ai
		node * temp=aux->prox;
		temp->ant=NULL;
		aux->prox=NULL;
		L->primeiro=temp;
		minhas_t[i].primeiro=L->primeiro;
	}
	printf("\n");
	pthread_t minhas_threads[quant_nucleos];
	for(int i=0;i<quant_nucleos;i++){
		pthread_create(&(minhas_threads[i]), NULL,insertionSort_list, (void *)&minhas_t[i]);
		printf("%dª Thread criada...\n",i+1);
		
	}
	for(int i=0; i<quant_nucleos; i++) {
		pthread_join(minhas_threads[i], NULL);
	}
	for(int i=0;i<quant_nucleos;i++){
		printf("\n");
		//mostra_lista(&minhas_t[i]);
		printf("\n");
	}


//criando o arquivo de saida

	FILE * arq=fopen("output.bin", "wb");
	if(arq == NULL){
		printf("Error ao abrir arquivo..\n");
		return;
	}
	
	for(int k=0;k<( ((int)atoi(tamanho)*size_file)/size_int );k++)
	{
		int menor=menor_das_listas(minhas_t);
		fwrite (&menor, sizeof(menor),1, arq);
	} 
	fclose(arq);


	
}


int main(){
	srand(time(NULL));
	if(SO == 1){
		retorna_nucleos_unix();
	}else{
		retorna_nucleos_dos();
	}
	//Após isso eu terei uma variavel Global <quant_nucleo> referênte a quantidade de nucleos te processador
	printf("Você tem %d nucleos de processamento...\n",quant_nucleos);
	Lista * L=cria_lista();
	char tamanho_dos_arquivos[]="1";
	cria_arquivos(L,tamanho_dos_arquivos);
	//depois daqui estaram todos os arquivos criados

	pthread_t threads[10];

	sem_init(&mutex, 0, 1);  
	
	char nome_arq[6];
	nome_arq[1]='.';
	nome_arq[2]='b';
	nome_arq[3]='i';
	nome_arq[4]='n';
	nome_arq[5]='\0';
	for(int i=0;i<10;i++){
		nome_arq[0]=atoi_reverse(i);
		Objeto_Thread * obj=(Objeto_Thread *)malloc(sizeof(Objeto_Thread));
		obj->L=L;
		obj->arquivo=nome_arq;
		obj->tamanho=tamanho_dos_arquivos;
		pthread_create(&(threads[i]), NULL,carrega_lista, (void *)obj);
	}
	for(int i=0; i<10; i++) {
		pthread_join(threads[i], NULL);
	}
	printf("Lista Completa:");
	sleep(1);
	//mostra_lista(L);
	ordena_lista(L,tamanho_dos_arquivos);
	return 0;
}
