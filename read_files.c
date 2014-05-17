#include <stdio.h>
#include <stdlib.h>

#define size_int 4

int main() {
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
	printf("Foram %d Inteiros e deste %d estão desordenados\n",quant_inteiros-1, quant_inteiros_des);
	fclose(arq);
	return 0;
}
