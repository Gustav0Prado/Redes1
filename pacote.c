#include "pacote.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @brief Divide e envia o arquivo indicado
 * 
 * @param arquivo Caminho para o arquivo a ser enviado
 */
void enviaArquivo(char *arquivo){
    FILE* arq;
    struct stat st;
    unsigned char buff[63];

    stat(arquivo, &st);
    long tamanho = st.st_size;

    arq = fopen(arquivo, "r");
    if(arq == NULL){
        printf("\tERRO ao abrir arquivo\n");
    }

    for(int i = 0; i < tamanho - tamanho%63; i+=63){
        fread(buff, sizeof(unsigned char), 63, arq);
    }
    fread(buff, sizeof(unsigned char), tamanho%63, arq);
}

/**
 * @brief Escreve partes de um arquivo
 * 
 * @param arquivo Caminho para o arquivo
 * @param dados Vetor com dados do pacote
 * @param tamanho Tamanho dos dados
 */
void escreveParte(char *arquivo, unsigned char *dados, int tamanho){
    FILE* arq;

    // Remove arquivo caso já exista
    if(access(arquivo, F_OK) == 0){
        remove(arquivo);
    }

    // Cria novo arquivo em modo append, para acumular os dados dos pacotes
    arq = fopen(arquivo, "a");
    if(arq == NULL){
        printf("\tERRO ao abrir arquivo\n");
    }

    // Escreve dados no arquivo
    fwrite(dados, sizeof(unsigned char), tamanho, arq);
}