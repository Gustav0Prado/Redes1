#include "utils.h"

void clearLines(){
   printf("\033[1A");
   for (int i = 0; i < 100; ++i){
      printf("\b");
   }
}

/**
 * @brief Divide e envia o arquivo indicado
 * 
 * @param arquivo Caminho para o arquivo a ser enviado
 */
void enviaArquivo(int socket, char *arquivo, seq_t *seq){
   FILE *arq;
   struct stat st;
   unsigned char buff[67], filename[67], buffer_resposta[67];
   int tam_read, progress = 0;
   pacote_t p, resposta;

   stat(arquivo, &st);
   long tamanho = st.st_size;

   // Manda nome do arquivo
   p.ini  = 126;
   p.seq  = (*seq).client;
   p.tipo = T_BACKUP_UM;
   p.tam  = strlen(arquivo);

   ((*seq).client) = (((*seq).client) + 1) % 64;

   memcpy(filename, &p, 3);
   strncpy((char *)filename+4, arquivo, 63);
   send(socket, filename, 67, 0);

   // Aguarda receber OK
   while(1){
      if (recv(socket, buffer_resposta, sizeof(buffer_resposta), 0) > 0){
         memcpy(&resposta, buffer_resposta, 3);
         if(resposta.ini == 126 && resposta.tipo == T_OK){
            break;
         }
      }
   }
   if(resposta.tipo == T_OK){
      p.tipo = T_DADOS;
      // Começa a mandar arquivo
      //printf("\t%d%%\n", progress);

      arq = fopen(arquivo, "r");
      if(arq == NULL){
         printf("\tERRO ao abrir arquivo\n");
      }

      for(int i = 0; i <= tamanho - tamanho%63; i+=63){
         // int received = 0;
         // while(!received){
            tam_read = fread(buff+4, sizeof(unsigned char), 63, arq);
            p.tam = tam_read;
            p.seq = (*seq).client;

            memcpy(buff, &p, 3);

            // clearLines();
            // printf("\tEnviando... %.2f%%\n", (progress++/ (float) (tamanho/63)) * 100);
            
            int s = send(socket, buff, 67, 0);
            printf("send: %02d - %02d - %03d\n", s, p.seq, ++progress);
            
            //Aguarda ACK
            while(1){
               if (recv(socket, buffer_resposta, sizeof(buffer_resposta), 0) > 0){
                  memcpy(&resposta, buffer_resposta, 3);
                  if(resposta.ini == 126 && resposta.tipo == T_ACK){
                     break;
                  }
               }
            }

            ((*seq).client) = (((*seq).client) + 1) % 64;
         // }
      }

      tam_read = fread(buff+4, sizeof(unsigned char), tamanho%63, arq);
      if(tam_read > 0){
         tam_read = fread(buff+4, sizeof(unsigned char), tamanho%63, arq);
         p.tam = tam_read;
         p.seq = (*seq).client;

         memcpy(buff, &p, 3);
      
         int s = send(socket, buff, 67, 0);
         printf("send: %02d - %02d - %03d\n", s, p.seq, ++progress);

         while(1){
               if (recv(socket, buffer_resposta, sizeof(buffer_resposta), 0) > 0){
                  memcpy(&resposta, buffer_resposta, 3);
                  if(resposta.ini == 126 && resposta.tipo == T_ACK){
                     break;
                  }
               }
            }

         ((*seq).client) = (((*seq).client) + 1) % 64;
      }

      fclose(arq);
   }
   else{
      printf("Respondeu com %d - %d\n", resposta.tipo, resposta.seq);
   }
}

/**
 * @brief Escreve partes de um arquivo
 * 
 * @param arquivo Caminho para o arquivo
 * @param dados Vetor com dados do pacote
 * @param tamanho Tamanho dos dados
 */
void escreveParte(char *arquivo, unsigned char *dados, int tamanho){
   FILE* arq;

   // Remove arquivo caso já exista => Como faz parte do arquivo, acaba deletando ele sempre
   //if(access(arquivo, F_OK) == 0){
   //    remove(arquivo);
   //}

   // Cria novo arquivo em modo append, para acumular os dados dos pacotes
   arq = fopen(arquivo, "a");
   if(arq == NULL){
      printf("Erro ao abrir arquivo: ");
      switch(errno){
         case ENOSPC:
            // Manda pacote com erro
            break;
         
         case EACCES:
            // Manda pacote com erro
            break;
      }
   }

   // Escreve dados no arquivo
   fwrite(dados, sizeof(unsigned char), tamanho, arq);

   fclose(arq);
}