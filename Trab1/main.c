#include "utils.h"

int main(int argc, char **argv){
   seq_t seq;
   seq.client = 0; seq.server = 0;
   unsigned char rcveD[134], rcve[67];
   char entrada[256];
   char delimitador[3] = " \n";
   char *token;

   int socket = ConexaoRawSocket("eno1");

   // Trata entrada
   escolheEntrada(argc, argv);

   memset(rcve, 0, 67);

   // Timeout de 5 segundo = 10000 milissegundos
   int timeoutMillis = 5000;
   struct timeval timeout = { .tv_sec = timeoutMillis / 1000, .tv_usec = (timeoutMillis % 1000) * 1000 };
   setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));

   pacote_t package;
   char filename[63], expr[63];
   char md5[63];
   int qtd_files = 0;
   int esc;

   if(servidor){
      while(1){

         if(recv(socket, rcveD, sizeof(rcveD), 0) > 0){
            int i = 0;
            int j = 0;
            while(i < 134){
               rcve[i] = rcveD[j];

               i++;
               j+=2;
            }

            memcpy(&package, rcve, 3);
            if(package.ini == 126){
               printf("\t\tRecebeu seq: %d\n", package.seq);
               if(rcve[66] != calcula_paridade(rcve, package.tam)){
                  envia(socket, NULL, 0, T_NACK, NULL, 0, 0, NULL);
               }
               else if(package.seq != seq.client){
                  // Mensagem já recebida anteriormente, apenas confirma
                  if( mensagem_anterior(servidor, &seq, package.seq, package.tipo) ){
                     envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
                  }
               }
               else{
                  switch (package.tipo){
                     case T_BACKUP_UM: //caso peça backup de um arquivo
                        strncpy(filename, basename((char *)rcve+3), package.tam);
                        if (access(filename, 0) == 0) // se o nome do arquivo requisitado já existe, remove
                        {
                           remove(filename);
                        }
                        
                        envia(socket, NULL, 0, T_OK, NULL, 0, 0, NULL);

                        break;

                     case T_BACKUP_VARIOS: //caso peça backup de um arquivo
                        qtd_files = rcve[4];
                        envia(socket, NULL, 0, T_OK, NULL, 0, 0, NULL);
                        
                        break;

                     case T_DADOS://caso esteja passando o pacote de dados
                        esc = escreveParte(filename, rcve+3, package.tam);
                        if (esc > 0){
                           escreveErro_e_envia_pkgerro(filename, esc, socket, &seq);
                        }
                        
                        envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);

                        break;

                     case T_FIM_ARQUIVO:
                        printf("\tArquivo %s recebido com sucesso\n", filename);
                        envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
                        break;

                     case T_FIM_GRUPO:
                        printf("\t%d arquivos recebidos com sucesso!\n", qtd_files);
                        envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
                        break;

                     case T_CD_REMOTO://pede para trocar o diretório do server

                        strncpy(filename, (char *)rcve+3, package.tam);
                        cdLocal(filename);

                        envia(socket, NULL, 0, T_OK, NULL, 0, 0, NULL);

                        break;

                     case T_RECUPERA_UM:
                        strncpy(filename, (char *)rcve+3, package.tam);

                        if (access(filename, 0) != 0){
                           if(errno == ENOENT){
                              unsigned char ans[63];
                              sprintf((char *)ans, "%d", ERRO_ARQ_NEXISTE);
                              envia(socket, ans, strlen((char *)ans)+1, T_ERRO, &seq, 0, 0, NULL);
                           }
                        }
                        else{
                           enviaArquivo(socket, filename, &seq);
                           envia(socket, NULL, 0, T_FIM_ARQUIVO, &seq, 0, 0, NULL);
                        }


                        break;

                     case T_RECUPERA_VARIOS:
                        strncpy(expr, (char *)rcve+3, package.tam);

                        enviaVariosArquivos(socket, expr, &seq);

                        envia(socket, NULL, 0, T_FIM_GRUPO, &seq, 0, 0, NULL);

                        break;

                     case T_VERIFICA_BACKUP:
                        strncpy(filename, (char *)rcve+3, package.tam);

                        // Se arquivo existe, manda o MD5
                        if( geraMD5(filename, (unsigned char *)md5) > 0){
                           envia(socket, (unsigned char *)md5, strlen(md5)+1, T_MD5, &seq, 0, 0, NULL);
                        }
                        else{
                           // Arquivo não existe, manda erro
                           char ans[63];
                           sprintf(ans, "%d", ERRO_ARQ_NEXISTE);
                           envia(socket, (unsigned char *)ans, strlen(ans)+1, T_ERRO, &seq, 0, 0, NULL);
                        }

                        break;

                     default:
                        break;
                  }

                  // printf("Ultimo recebido %d", seq.client);
                  seq.client = (seq.client + 1) % 64;
                  // printf(" -- espera %d\n", seq.client);
               }
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
         if((token) && strlen(token) > 63){
            printf("Erro ao ler token! Token muito grande ( > 63 caracteres)\n");
         }
         else{
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
                  checaMD5(socket, token, &seq);
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
}
