#include "utils.h"

int main(int argc, char **argv){
   int servidor;
   seq_t seq;
   seq.client = 0; seq.server =0;
   unsigned char rcve[67];
   char entrada[256];
   char delimitador[3] = " \n";
   char *token;
   char lixo;
   pacote_t resposta;
   unsigned char buffer_resposta[67];

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

   memset(rcve, 0, 67);

   // Timeout de 10 segundos = 10000 milissegundos
   //int timeoutMillis = 10000;
   //struct timeval timeout = { .tv_sec = timeoutMillis / 1000, .tv_usec = (timeoutMillis % 1000) * 1000 };
   //setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));

   pacote_t package;
   char *filename;
   int cont = 0;
   if(servidor){
      while(1){

         if(recv(socket, rcve, 67, 0) > 0){
            memcpy(&package, rcve, 3);
            //printf("o rcve[0] é %d\n ", rcve[0]);
            if(package.ini == 126 && package.seq == seq.client){
               
               printf( "Recebeu %d pacotes e o último recebido foi %d \n", cont++, package.seq);

               switch (package.tipo)
               {
               case T_BACKUP_UM: //caso peça backup de um arquivo
                  filename = malloc (63);
                  strncpy(filename, (char *)rcve+4, package.tam);
                  printf( "Pediu backup de %s\n", filename);
                  if (access(filename, 0) == 0) // se o nome do arquivo requisitado já existe, remove
                  {
                     int retorno = remove(filename);
                     printf("tentou remover e voltou %d\n", retorno);
                  }
                  resposta.ini = 126;
                  resposta.tipo = T_OK;
                  resposta.seq = 0;
                  memcpy(buffer_resposta, &resposta, 3);

                  send(socket, buffer_resposta, sizeof(buffer_resposta) , 0);
                  break;
               case T_DADOS://caso esteja passando o pacote de dados
                  FILE *arq = fopen(filename, "a+");
                  fwrite(rcve+4, sizeof(unsigned char), package.tam, arq);
                  fclose(arq);
                  resposta.ini = 126;
                  resposta.tipo = T_ACK;
                  resposta.seq = 3;
                  memcpy(buffer_resposta, &resposta, 3);
                  send(socket, buffer_resposta, sizeof(buffer_resposta) , 0);

                  break;
               default:
                  break;
               }

               seq.client = (seq.client + 1) % 64;
            }
         }

      }
   }
   else{
      while(1){
         printf(">>> ");
         if(fgets(entrada, 63, stdin) == NULL){
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
               backup1Arquivo(socket, token, &seq);
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
