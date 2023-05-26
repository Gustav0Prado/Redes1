#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include "socket.h"
#include "comandos.h"
#include "pacote.h"

#define MAX_TAM 64

int main(int argc, char **argv){
   int servidor;
   int sequencia = 0;
   unsigned char msg[MAX_TAM];
   unsigned char rcve[MAX_TAM];
   char entrada[MAX_TAM];
   char delimitador[3] = " \n";
   char *token;
   char lixo;

   int socket = ConexaoRawSocket("lo");

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

   memset(rcve, 0, MAX_TAM);

   if(servidor){
      // Setar timeout para um segundo
      // struct timeval tv;
      // tv.tv_sec = 2;
      // tv.tv_usec = 0;
      // setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

      while(1){
         if( recv(socket, rcve, sizeof(rcve), 0) ){
            if(rcve[0] == 126 && rcve[1] == sequencia){
               printf("\tRecebeu mensagem: %.*s", MAX_TAM-2, rcve+2);
               sequencia++;
            }
         }
      }
   }
   else{
      while(1){
         fgets(entrada+2, MAX_TAM-2, stdin);
         entrada[0] = 126;
         entrada[1] = sequencia;
         sequencia++;
         printf("sended: %ld\n", send(socket, entrada, MAX_TAM, 0));
         //token = strtok(entrada, delimitador);

         // int codigo;
         // if(token)
         //    codigo = codigoComando(token);
         // else
         //    codigo = -1;

         // token = strtok(NULL, delimitador);

         // switch (codigo){
         //    case CDLOCAL:
         //       cdLocal(token);
         //       break;
            
         //    case BACKUP_UM:
         //       backup1Arquivo(token);
         //       break;

         //    case BACKUP_VARIOS:
         //       backupVariosArquivo(token);
         //       break;

         //    case LS:
         //       system("ls");
         //       break;

         //    default:
         //       printf("ERRO: Comando desconhecido\n");
         //       break;
         // }
      }
   }
}
