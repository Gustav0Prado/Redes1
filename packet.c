#include "packet.h"
#include <string.h>
#include <stdio.h>

int main(){
    unsigned char pacoteChar[5];
    pacote_t pacote;
    pacote.marcador  = 126;
    pacote.tamanho   = 15;
    pacote.sequencia = 20;
    pacote.tipo      = 4;
    pacote.paridade  = 13;

    for(int i = 7; i >= 0; i--) putchar('0' + ((pacote.marcador >> i) & 1));
    printf(" ");

    for(int i = 5; i >= 0; i--) putchar('0' + ((pacote.tamanho >> i) & 1));
    printf(" ");

    for(int i = 5; i >= 0; i--) putchar('0' + ((pacote.sequencia >> i) & 1));
    printf(" ");

    for(int i = 3; i >= 0; i--) putchar('0' + ((pacote.tipo >> i) & 1));
    printf(" ");

    for(int i = 7; i >= 0; i--) putchar('0' + ((pacote.paridade >> i) & 1));
    printf("\n");

    pacoteChar[0] = pacote.marcador;
    pacoteChar[1] = pacote.tamanho;
    pacoteChar[2] = pacote.sequencia;
    pacoteChar[3] = pacote.tipo;
    pacoteChar[4] = pacote.paridade;
    
    for(int i = 0; i < sizeof(pacoteChar); ++i){
        printf("%d ", pacoteChar[i]);
    }
    printf("\n");

    for(int i = 7; i >= 0; i--) putchar('0' + ((pacoteChar[0] >> i) & 1));
    printf(" ");

    for(int i = 5; i >= 0; i--) putchar('0' + ((pacoteChar[1] >> i) & 1));
    printf(" ");

    for(int i = 5; i >= 0; i--) putchar('0' + ((pacoteChar[2] >> i) & 1));
    printf(" ");

    for(int i = 3; i >= 0; i--) putchar('0' + ((pacoteChar[3] >> i) & 1));
    printf(" ");

    for(int i = 7; i >= 0; i--) putchar('0' + ((pacoteChar[4] >> i) & 1));
    printf(" ");

    printf("\n");
}
