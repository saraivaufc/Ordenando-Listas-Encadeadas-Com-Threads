//BIBLIOTECAS NECESSARIAS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "lista.h"
#include <semaphore.h>

//MEUS DEFINES
#define quant_arquivos 10  //a quantidade de arquivos binario que eu desejar
#define size_int 4   // o tamanho em Bytes dos arquivos que eu vou trabalhar, se fossem caracteres seria 1
#define size_file 1024 //tamanho de Bytes que temos em 1 Kb

//MINHAS ESTRUTURAS
typedef struct{
	Lista * L;//lista onde armazenará os valores
	char * arquivo;//o path do arquivo que eu vou jogar na lista
	int tamanho;//quantidade de KB do arquivo
}Objeto_Thread;

//MINHAS VARIAVEIS GLOBAIS
sem_t mutex; //crio uma variavel Mutex da semaphore.h
int quant_nucleos; //essa variavel armazenará a quantidade de nucleos da minha CPU
int counter; //
unsigned long long tam_output=0;

//MINHA FUNÇÔES
void retorna_nucleos_unix(); //essa função armazena em <quant_nucleos> a quantidade de nucleo da CPU onde o SO seja baseado no Unix 
void retorna_nucleos_dos(); //essa função armazena em <quant_nucleos> a quantidade de nucleo da CPU onde o SO seja baseado no MS-DOS
char atoi_reverse(int n); //essa  função recebe um numero inteiro e retorna uma string referente aquele numero
void cria_arquivos(Lista *L,int tamanho); //cria <quant_arquivos> binarios com inteiros aleatorios
void * carrega_lista(void * obj); //essa função recebe de todos os arquivos os valores e inserem na lista, usando threads
void * insertionSort_list(void *arg); //ordena uma lista encadeada usando o algoritmo Insertio-Sort
int menor_das_listas(Lista *l, FILE * arq, FILE * saida); //recebe um vetor de listas ordenadas e remove e retorna o menor valor delas
void ordena_lista(Lista * L, int tamanho); //essa função gerencia as threads na hora da ordenação das listas
void cria_saida(Lista * minhas_t); //essa função faz a magia rsrsr, ela entrelaça N listas e joga automaticamente em output.bin 

//FUNÇÔES IMPLEMENTADAS
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

void cria_arquivos(Lista *L,int tamanho){
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
		for(int k=0;k<(tamanho*size_file)/size_int;k++)
		{
			int aleatorio=rand();
			fwrite (&aleatorio, sizeof(aleatorio),1, arq);
		}
		fclose(arq);
	}
}

int cont=0;
void * carrega_lista(void * obj){
	printf("\t%dª Thread criada...\n",++cont);
	Objeto_Thread * Objeto=(Objeto_Thread *) obj;
	FILE * arq=fopen(Objeto->arquivo, "rb");
	if(arq == NULL){
		printf("Error ao abrir o arquivo...\n");
		return NULL;
	}
	for(int k=0;k<(Objeto->tamanho*size_file)/size_int;k++)
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

int menor_das_listas(Lista *l, FILE * arq, FILE * saida){
	int menor;
	//se eu tiver apenas uma lista
	if(quant_nucleos == 1){
		for(node * aux = l->primeiro; aux != NULL; aux=aux->prox){
			menor=aux->valor;
			if(saida != NULL)
			fprintf(saida,"[%d]",menor);
			tam_output++;
			fwrite (&menor, sizeof(menor),1, arq);
		}
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



void ordena_lista(Lista * L, int tamanho){
	int pedaco=(int)L->tam/quant_nucleos;
	Lista minhas_t[quant_nucleos];
	
	minhas_t[0].primeiro=L->primeiro;
	for(int i=1;i<quant_nucleos;i++){
		
		node * aux=L->primeiro;
		for(int k=0;k<pedaco;k++){
			aux=aux->prox;
		}
		node * temp=aux->prox;
		temp->ant=NULL;
		aux->prox=NULL;
		L->primeiro=temp;
		minhas_t[i].primeiro=L->primeiro;
	}
	printf("Agora criar as threads para ordenar a lista...\n");
	pthread_t minhas_threads[quant_nucleos];
	for(int i=0;i<quant_nucleos;i++){
		pthread_create(&(minhas_threads[i]), NULL,insertionSort_list, (void *)&minhas_t[i]);
		printf("\t%dª Thread criada...\n",i+1);
		sleep(1);
	}
	for(int i=0; i<quant_nucleos; i++) {
		pthread_join(minhas_threads[i], NULL);
	}
	
	//comentei isso para evitar imprimir as lists na tela
	/*
	for(int i=0;i<quant_nucleos;i++){
		printf("\n");
		mostra_lista(&minhas_t[i]);
		printf("\n\n");
	}
	*/


	//criando o arquivo de saida
	cria_saida(minhas_t);
}

void cria_saida(Lista * minhas_t){
	FILE * arq=fopen("output.bin", "wb");
	if(arq == NULL){
		printf("Error ao abrir arquivo..\n");
		return;
	}
	
	//altere as opções caso queria um .TXT do OUTPUT.BIN
	//FILE * saida=fopen("saida.txt","w+");
	FILE * saida=NULL;
	
	
	for( ; ; )
	{
		int menor=menor_das_listas(minhas_t,arq,saida);
		if(menor == -1)
			return;
		if(saida != NULL)
			fprintf(saida,"[%d]",menor);
		tam_output++;
		fwrite (&menor, sizeof(menor),1, arq);
	} 
	fclose(saida);
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
	printf("Digite o tamanho do seu arquivo em KB:");
	int tamanho_dos_arquivos;
	scanf("%d",&tamanho_dos_arquivos);
	
	time_t inicio, fim;
	inicio=time(NULL);
	
	cria_arquivos(L,tamanho_dos_arquivos);
	//depois daqui estaram todos os arquivos criados

	pthread_t threads[10];

	//inicializo meu Mutex
	sem_init(&mutex, 0, 1);
	
	char nome_arq[6];
	nome_arq[1]='.';
	nome_arq[2]='b';
	nome_arq[3]='i';
	nome_arq[4]='n';
	nome_arq[5]='\0';
	
	printf("Criar as Threads para receber os arquivos...\n");
	for(int i=0;i<10;i++){
		nome_arq[0]=atoi_reverse(i);
		
		//Para usar todas as minha variaveis na thread, eu preciso compactala em apenas uma Struct, assim posso passar um *void
		Objeto_Thread * obj=(Objeto_Thread *)malloc(sizeof(Objeto_Thread));
		obj->L=L;
		obj->arquivo=nome_arq;
		obj->tamanho=tamanho_dos_arquivos;
		
		
		pthread_create(&(threads[i]), NULL,carrega_lista, (void *)obj);
	}
	for(int i=0; i<10; i++) {
		pthread_join(threads[i], NULL);
	}
	ordena_lista(L,tamanho_dos_arquivos);
	fim=time(NULL);
	printf("O tamanho da sua lista é: %lld inteiros",tam_output);
	
	int tempo=(difftime(inicio,fim) * (-1) );
	FILE * log=fopen("log.txt","w+");
	fprintf(log,"Segundos: %d ou Minutos: %d\n",tempo,tempo/60);
	printf("\nSegundos: %d ou Minutos: %d\n",tempo,tempo/60);
	fclose(log);
	return 0;
}
