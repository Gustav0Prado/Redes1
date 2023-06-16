#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "socket.h"
#include "comandos.h"
#include "pacote.h"

int main(int argc, char **argv){
   int servidor;
   int sequencia = 0;
   unsigned char msg[256];
   unsigned char rcve[63];
   char entrada[256];
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

   memset(rcve, 0, 63);

   if(servidor){
      while(1){
         if(recv(socket, rcve, 63, 0) > 0){
            printf("recebeu: '%s'\n", rcve);

            FILE *arq = fopen("ola.txt", "w");
            fwrite(rcve, sizeof(unsigned char), 63, arq);
            fclose(arq);
            memset(rcve, 0, 63);
         }
      }
   }
   else{
      while(1){
         printf(">>> ");
         fgets(entrada, 256, stdin);
         token = strtok(entrada, delimitador);

         int codigo;
         if(token)
            codigo = codigoComando(token);
         else
            codigo = -1;

         token = strtok(NULL, "\n");

         switch (codigo){
            case CDLOCAL:
               cdLocal(token);
               break;
            
            case BACKUP_UM:
               backup1Arquivo(socket, token);
               break;

            case BACKUP_VARIOS:
               backupVariosArquivos(token);
               break;

            case LS:
               system("ls -l");
               break;

            default:
               printf("ERRO: Comando desconhecido\n");
               break;
         }
      }
   }
}
