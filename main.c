#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "socket.h"
#include "comandos.h"

int main(int argc, char **argv){
   int servidor;
   unsigned char msg[256];
   unsigned char rcve[256];
   char entrada[1024];
   char delimitador[3] = " \n";
   char *token;
   char lixo;

   int socket = ConexaoRawSocket("eno1");

   // Trata entrada
   if(argc == 1){
      printf("0 - Cliente\n1 - Servidor\n");
      scanf("%d", &servidor);
      scanf("%c", &lixo);
      system("clear");
   }
   else{
      if(strcmp(argv[1], "cliente") == 0){
         system("clear");
         printf("Iniciado como cliente\n");
         servidor = 0;
      }
      else if(strcmp(argv[1], "servidor") == 0){
         system("clear");
         printf("Iniciado como servidor\n");
         servidor = 1;
      }
      else{
         printf("Por favor selecione uma opção válida:\n");
         printf("0 - Cliente\n1 - Servidor\n");
         scanf("%d", &servidor);
         scanf("%c", &lixo);
         system("clear");
      }
   }

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
         int codigo = codigoComando(token);
         token = strtok(NULL, delimitador);

         switch (codigo){
            case CDLOCAL:
               cdLocal(token);
               break;
            
            case BACKUP_UM:
               backup1Arquivo(token);
               break;

            case BACKUP_VARIOS:
               backupVariosArquivo(token);
               break;

            default:
               printf("ERRO: Comando desconhecido\n");
               break;
         }
      }
   }
}
