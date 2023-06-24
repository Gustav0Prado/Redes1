#include "utils.h"

int main(int argc, char **argv){
   seq_t seq;
   seq.client = 0; seq.server =0;
   unsigned char rcve[67];
   char entrada[256];
   char delimitador[3] = " \n";
   char *token;

   int socket = ConexaoRawSocket("enp3s0");

   // Trata entrada
   escolheEntrada(argc, argv);

   memset(rcve, 0, 67);

   // Timeout de 10 segundos = 10000 milissegundos
   int timeoutMillis = 1000;
   struct timeval timeout = { .tv_sec = timeoutMillis / 1000, .tv_usec = (timeoutMillis % 1000) * 1000 };
   setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));

   pacote_t package;
   char filename[63], expr[63];
   int qtd_files = 0;

   if(servidor){
      while(1){

         if(recv(socket, rcve, 67, 0) > 0){
            memcpy(&package, rcve, 3);
            //printf("o rcve[0] é %d\n ", rcve[0]);
            if(package.ini == 126 && package.seq == seq.client){
               //CHECAR PARIDADE!!!!
               //printf( "Recebeu tipo %d \n", package.tipo);

               switch (package.tipo){
                  case T_BACKUP_UM: //caso peça backup de um arquivo
                     strncpy(filename, basename((char *)rcve+4), package.tam);
                     //printf( "Pediu backup de %s\n", filename);
                     if (access(filename, 0) == 0) // se o nome do arquivo requisitado já existe, remove
                     {
                        int retorno = remove(filename);
                        //printf("tentou remover e voltou %d\n", retorno);
                     }
                     
                     envia(socket, NULL, 0, T_OK, NULL, 0, 0);

                     break;

                  case T_BACKUP_VARIOS: //caso peça backup de um arquivo
                     qtd_files = rcve[4];
                     envia(socket, NULL, 0, T_OK, NULL, 0, 0);
                     
                     break;

                  case T_DADOS://caso esteja passando o pacote de dados
                     FILE *arq = fopen(filename, "a+");
                     fwrite(rcve+4, sizeof(unsigned char), package.tam, arq);
                     fclose(arq);
                     
                     envia(socket, NULL, 0, T_ACK, NULL, 0, 0);

                     break;

                  case T_FIM_ARQUIVO:
                     printf("\tArquivo %s recebido com sucesso\n", filename);
                     envia(socket, NULL, 0, T_ACK, NULL, 0, 0);
                     break;

                  case T_FIM_GRUPO:
                     printf("\t%d arquivos recebidos com sucesso!\n", qtd_files);
                     envia(socket, NULL, 0, T_ACK, NULL, 0, 0);
                     break;

                  case T_CD_REMOTO://pede para trocar o diretório do server

                     strncpy(filename, (char *)rcve+4, package.tam);
                     cdLocal(filename);

                     envia(socket, NULL, 0, T_OK, NULL, 0, 0);

                     break;

                  case T_RECUPERA_UM:
                     strncpy(filename, (char *)rcve+4, package.tam);

                     if (access(filename, 0) != 0){
                        if(errno == ENOENT){
                           envia(socket, NULL, 0, ERRO_ARQ_NEXISTE, NULL, 0, 0);
                        }
                     }
                     else{
                        envia(socket, NULL, 0, T_ACK, NULL, 0, 0);
                        enviaArquivo(socket, filename, &seq);
                        envia(socket, NULL, 0, T_FIM_ARQUIVO, &seq, 0, 0);
                     }

                     break;

                  case T_RECUPERA_VARIOS:
                     strncpy(expr, (char *)rcve+4, package.tam);

                     enviaVariosArquivos(socket, expr, &seq);

                     envia(socket, NULL, 0, T_FIM_GRUPO, &seq, 0, 0);

                     break;

                  default:
                     break;
               }

               seq.client = (seq.client + 1) % 64;
            }
            else if(package.ini == 126 && package.seq != seq.client){
               envia(socket, NULL, 0, T_NACK, NULL, 0, 0);
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

            case CDREMOTO:
               cdRemoto(socket, token, &seq);
               break;
            
            case BACKUP_UM:
               backup1Arquivo(socket, token, &seq);
               break;

            case BACKUP_VARIOS:
               enviaVariosArquivos(socket, token, &seq);
               break;

            case RESTAURA_UM:
               restaura1Arquivo(socket, token, &seq);
               break;

            case RESTAURA_VARIOS:
               restauraVariosArquivos(socket, token, &seq);
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
