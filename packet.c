#include "packet.h"
#include <string.h>
#include <stdio.h>

int main(){
    unsigned char pacoteChar[4];
    pacote_t pacote, pacote2;
    pacote.marcador  = 126;
    pacote.tamanho   = 0;
    pacote.sequencia = 0;
    pacote.tipo      = 2;
    pacote.paridade  = 0;

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

    memcpy(pacoteChar, &pacote, 4);
    memcpy(&pacote2, pacoteChar, 4);
    
    for(int i = 7; i >= 0; i--) putchar('0' + ((pacoteChar[0] >> i) & 1));
    printf(" ");

    for(int i = 5; i >= 0; i--) putchar('0' + ((pacoteChar[1] >> i) & 1));
    printf(" ");

    for(int i = 1; i >= 0; i--) putchar('0' + ((pacoteChar[1] >> i) & 1));
    printf(" ");

    printf("\n");
}