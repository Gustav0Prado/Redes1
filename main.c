#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "socket.h"

int main(){
   int servidor;
   unsigned char msg[256];
   unsigned char rcve[256];
   char lixo;

   int socket = ConexaoRawSocket("enp0s31f6");

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
         fgets((char *)msg, 256, stdin);
         send(socket, msg, 256, 0);
      }
   }
}
