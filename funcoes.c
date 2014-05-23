#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include "quick.h"
#include <pthread.h>


#define quant_arquivos 10

//minha var globais
extern int quant_nucleos;
pthread_mutex_t count_mutex;
pthread_t threads[quant_arquivos/2];

pthread_mutex_t count_mutex2;
pthread_t threads2[quant_arquivos/2];
int saiu_menor=0;

void * carrega_lista_threads(void * obj);

char atoi_reverse(int n){
	char c=(char)n+'0';
	return c;
}

void retorna_nucleos_unix(){
	system("cat /proc/cpuinfo | grep processor | wc -l  >> nucleos.txt");
	FILE * nucleos;
	nucleos=fopen("nucleos.txt" , "r+");
	fscanf(nucleos,"%d",&quant_nucleos);
	fclose(nucleos);
	system("rm nucleos.txt");
}

void retorna_nucleos_dos(){
	quant_nucleos = 4;
}

int cont=0;

void swap(int *a, int *b){
	int c=*a;
	*a=*b;
	*b=c;
}

void carrega_lista(Lista *L){
	Lista * L2=Cria_Lista();
	
	char nome_arq[22];
	char arqv[]="arquivos/arquivo";
	for(int i=0;i<16;i++){
		nome_arq[i]=arqv[i];
	}
	nome_arq[17]='.';
	nome_arq[18]='b';
	nome_arq[19]='i';
	nome_arq[20]='n';
	nome_arq[21]='\0';
	
	printf("Carregando Arquivos na Lista...\n");
	for(int i=0;i<quant_arquivos/2;i++){
		nome_arq[16]=atoi_reverse(i);
		Objeto_Thread * obj=(Objeto_Thread *)malloc(sizeof(Objeto_Thread));
		obj->L=L;
		obj->arquivo=nome_arq;
		obj->mutex=&count_mutex;
		
		pthread_create(&(threads[i]), NULL,carrega_lista_threads, (void *)obj);
	}
	for(int i=0;i<quant_arquivos/2;i++){
		nome_arq[16]=atoi_reverse(i+5);
		Objeto_Thread * obj=(Objeto_Thread *)malloc(sizeof(Objeto_Thread));
		obj->L=L2;
		obj->arquivo=nome_arq;
		obj->mutex=&count_mutex2;

		pthread_create(&(threads2[i]), NULL,carrega_lista_threads, (void *)obj);
	}
	
	for(int i=0; i<quant_arquivos/2; i++) {
		pthread_join(threads[i], NULL);
		pthread_join(threads2[i], NULL);
	}
	L->ultimo->prox=L2->primeiro;
	L2->primeiro->ant=L->ultimo;
	L->ultimo=L2->ultimo;
	L->n+=L2->n;
	printf("Todos os Arquivos Recebidos...\n\n");
}

void * carrega_lista_threads(void * obj){
	printf("\t%dª Thread criada...\n",++cont);
	Objeto_Thread * Objeto=(Objeto_Thread *) obj;
	FILE * arq=fopen(Objeto->arquivo, "r");
	if(arq == NULL){
		printf("Error ao abrir o arquivo...\n");
		return NULL;
	}
	while(1){
		int valor;
		fread(&valor, sizeof(valor),1, arq);
		
		pthread_mutex_lock(Objeto->mutex);
		Insere(Objeto->L,valor);
		
		if(feof(arq)){
			Nodo * aux=Objeto->L->primeiro;
			Objeto->L->primeiro=Objeto->L->primeiro->prox;
			Objeto->L->n--;
			free(aux);

			pthread_mutex_unlock(Objeto->mutex);

			break;
		}

		pthread_mutex_unlock(Objeto->mutex);
		
	}
	
	fclose(arq);
	return NULL;
}

Lista * particiona_lista(Lista *L){
	printf("Tamanho da Lista Geral: %d\n\n",L->n+L->n2);
	int pedaco=(int)( L->n-10)/quant_nucleos;
	Lista * minhas_t=(Lista *) malloc(quant_nucleos * sizeof(Lista));
	minhas_t[0].primeiro=L->primeiro;
	minhas_t[0].n=pedaco;
	for(int i=1;i<quant_nucleos;i++){
		Nodo * aux=L->primeiro;
		int counter=0;
		for(int k=0;k<pedaco;k++){
			aux=aux->prox;
			counter++;
		}
		minhas_t[i-1].ultimo=aux;
		
		Nodo * temp=aux->prox;
		temp->ant=NULL;
		aux->prox=NULL;
		L->primeiro=temp;
		minhas_t[i].primeiro=L->primeiro;
		minhas_t[i].n=counter;
	}
	minhas_t[quant_nucleos-1].ultimo=L->ultimo;

	return minhas_t;
}



int count=0;


void * ordena_lista_threads(void * arg){
	printf("\t%dª Thread de Ordenação Criada...\n",++count);
	Lista *L=(Lista *) arg;
	ordena(L);
	return arg;
}
	
void ordena_lista(Lista *Vetor){
	pthread_t threads_ordenacao[quant_nucleos];
	for(int i=0;i<quant_nucleos;i++){
		pthread_create(&(threads_ordenacao[i]), NULL,ordena_lista_threads, (void *)&Vetor[i]);
	}
	
	for(int i=0;i<quant_nucleos;i++){
		pthread_join(threads_ordenacao[i],NULL);
	}
	printf("Entrelaçando as Partes...\n");
	return;
}

int menor_das_listas(Lista l[], FILE * arq){
	int menor;
	//se eu tiver apenas uma lista
	if(quant_nucleos == 1){
		for(Nodo * aux = l->primeiro; aux != NULL; aux=aux->prox){
			menor=aux->valor;
			tam_output++;
			fwrite (&menor, sizeof(menor),1, arq);
		}
		saiu_menor=1;
		return -1;
	}
	
	//caso eu tenha mais de uma lista eu faço o menor ser o primeiro valor da primeira lista
	int temp=0;
	if(l[0].primeiro != NULL)
		menor=l[0].primeiro->valor;
	
	//crio um laço pra percorrer cada vetor de lista
	for(int i=1 ; i<quant_nucleos ; i++){
		//para toda lista não vazia faça
		if(l[i].primeiro != NULL){
			//se o menor valor dessa lista for menor que menor
			if(l[i].primeiro->valor < menor){
				//atualizo o temp e o menor
				temp=i;
				menor=l[i].primeiro->valor;
			}
		}
	}
	//eu sempre removo o menor de sua lista
	if(l[temp].primeiro != NULL)
		l[temp].primeiro=l[temp].primeiro->prox;
	
	//para todos os vetores faça
	for(int i=0 ; i<quant_nucleos ; i++){
		//se alguma lista estiiver vazia então
		if(l[i].primeiro == NULL && i<quant_nucleos-1){
			//tal lista recebe a lista da ultima possição e elmina a ultima posição
			l[i].primeiro=l[quant_nucleos-1].primeiro;
			quant_nucleos--;
		}
	}
	return menor; 
}

void cria_saida(Lista * L){
	FILE * arq=fopen("arquivos/output.bin", "wb");
	if(arq == NULL){
		printf("Error ao abrir arquivo..\n");
		return;
	}
	
	for( ; ; )
	{
		int menor=menor_das_listas(L,arq);
		if(saiu_menor == 1)
			break;
		tam_output++;
		fwrite (&menor, sizeof(menor),1, arq);
	}
	fclose(arq);
}

int esta_ordenado(){
	char path[] = "arquivos/output.bin";
	FILE *arq = fopen(path, "r");
	
	int quant_inteiros=0;
	int quant_inteiros_des=0;
	int valor_anterio;
	int valor_atual;
	fread(&valor_atual, sizeof(valor_atual),1, arq);
	quant_inteiros++;
	
	while(!(feof(arq))){
		valor_anterio=valor_atual;
		fread(&valor_atual, sizeof(valor_atual),1, arq);
		quant_inteiros++;
		if(valor_anterio > valor_atual){
			quant_inteiros_des++;
		}
	}
	fclose(arq);
	return quant_inteiros_des-1;//retorno um elemento a menos devido a função feof()
}
