#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "comandos.h"
#include "pacote.h"

/**
 * @brief Retorna codigo correspondente a string de um comando
 * 
 * @param token String com comando
 * @return int Codigo do comando
 */
int codigoComando(char *token){
    if(strcmp(token, "cdlocal") == 0)       return CDLOCAL;
    if(strcmp(token, "backup_um") == 0)     return CDLOCAL;
    if(strcmp(token, "backup_varios") == 0) return CDLOCAL;
    else return -1;
}

/**
 * @brief Muda para o diretorio indicado pelo caminho
 * 
 * @param caminho Caminho _ para o diretório desejado
 */
void cdLocal(char *caminho){
    if(caminho){
        if(chdir(caminho) == 0){
            printf("\tDiretorio mudado para %s\n", getcwd (NULL, 0));
        }
        else{
            printf("\tERRO ao mudar diretorio\n");
        }
    }
    else{
        printf("\tERRO ao ler caminho\n");
    }
}

/**
 * @brief Envia o backup de um arquivo para o servidor
 * 
 * @param arquivo Caminho para o arquivo com o backup a ser feito
 */
void backup1Arquivo(char *arquivo){
    // Checa se arquivo existe
    if(access(arquivo, F_OK) == 0){
        //envia mensagem de inicio com o nome do arquivo
        //espera ok
        enviaArquivo(arquivo);
        //envia fim
    }
}

/**
 * @brief Realiza backup de varios arquivos dada uma expressão regular
 * 
 * @param regex Expressão Regular dos arquivos desejados
 */
void backupVariosArquivo(char *regex){
    /*
        Acessa diretorio
        Para cada arquivo
            Checa se primeira parte satisfaz regex
            Se sim, checa se extensao satisfaz regex
                Se sim, backup1Arquivo
            Passa pro proximo
    */
}