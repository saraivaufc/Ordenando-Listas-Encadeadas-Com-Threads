#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#define size_int 4

char atoi_reverse(int n){
	char c=(char)n+'0';
	return c;
}

void cria_arquivos(int quant_arquivos, int  tamanho){
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

	srand(time(NULL));
	for(int i=0 ; i < quant_arquivos ; i++){
		nome_arq[16]=(char)atoi_reverse(i);
		FILE * arq=fopen(nome_arq, "wb");
		if(arq == NULL){
			printf("Error ao abrir arquivo..\n");
			return;
		}
		for(int k=0;k<(tamanho*1024)/size_int;k++)
		{
			int aleatorio=rand();
			fwrite (&aleatorio, sizeof(aleatorio),1, arq);
		}
		fclose(arq);
	}
}

int main(){
	int quant;
	printf("Digite a quantidade de Arquivos:");
	scanf("%d",&quant);
	int tam;
	printf("Digite o tamanho em KB:");
	scanf("%d",&tam);
	cria_arquivos(quant,tam);
	return 0;
}
