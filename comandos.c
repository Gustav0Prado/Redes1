#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "comandos.h"

/**
 * @brief Retorna codigo correspondente a string de um comando
 * 
 * @param token String com comando
 * @return int Codigo do comando
 */
int codigoComando(char *token){
    if(strcmp(token, "cdlocal") == 0) return CDLOCAL;
    else return -1;
}

/**
 * @brief Muda para o diretorio indicado pelo caminho
 * 
 * @param caminho Caminho _ para o diretório desejado
 * @return int Retorna 0 em caso de sucesso e -1 em caso de falha (erro ao mudar ou caminho invalido)
 */
int cdLocal(char *caminho){
    if(caminho){
        int ret = chdir(caminho);
        if(ret == 0){
            printf("\tDiretorio mudado para %s\n", getcwd (NULL, 0));
        }
        return ret;
    }
    return -2;
}