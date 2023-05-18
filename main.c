#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "socket.h"
#include "comandos.h"

int main(){
   int servidor;
   unsigned char msg[256];
   unsigned char rcve[256];
   char entrada[1024];
   char delimitador[3] = " \n";
   char *token;
   char lixo;

   int socket = ConexaoRawSocket("eno1");

   printf("0 - Cliente\n1 - Servidor\n");
   scanf("%d", &servidor);
   scanf("%c", &lixo);

   memset(rcve, 0, 256);

   if(servidor){
      while(1){
         if(recv(socket, rcve, 256, 0) != -1){
            printf("%s", rcve);
            memset(rcve, 0, 256);
         }
      }
   }
   else{
      while(1){
         fgets(entrada, 1024, stdin);
         token = strtok(entrada, delimitador);
         switch (codigoComando(token)){
         case CDLOCAL:
            token = strtok(NULL, delimitador);
            cdLocal(token);
            break;
         
         default:
            break;
         }
      }
   }
}
