#include "utils.h"

int main(int argc, char **argv){
   int servidor;
   int sequencia = 0;
   unsigned char rcve[67];
   char entrada[256];
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

   memset(rcve, 0, 67);

   if(servidor){
      while(1){
         if(recv(socket, rcve, 67, 0) > 0){
            if(rcve[0] == 126 && rcve[2] == sequencia){
               FILE *arq = fopen("./hello", "a");
               fwrite(rcve+4, sizeof(unsigned char), (int)rcve[1], arq);
               fclose(arq);

               sequencia = (sequencia + 1) % 256;
            }
         }
      }
   }
   else{
      while(1){
         printf(">>> ");
         if(fgets(entrada, 256, stdin) == NULL){
            exit(1);
         }
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
               backup1Arquivo(socket, token, &sequencia);
               break;

            case BACKUP_VARIOS:
               backupVariosArquivos(token);
               break;

            case LS:
               system("ls -l");
               break;
            
            case MD5:
               unsigned char md5[1024];
               int len = geraMD5(token, md5);

               //Printa MD5 na tela
               for(int i = 0; i < len; i++){
                  printf("%02x", md5[i]);
               }
               printf("  %s\n", token);
               break;

            case QUIT:
               exit(0);
               break;

            default:
               printf("ERRO: Comando desconhecido\n");
               break;
         }
      }
   }
}
