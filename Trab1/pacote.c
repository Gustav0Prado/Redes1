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
void enviaArquivo(int socket, char *arquivo, int *seq){
   FILE* arq;
   struct stat st;
   unsigned char buff[67];
   int tam_read, progress = 0;

   buff[0] = 126;
   buff[3] = 5;

   stat(arquivo, &st);
   long tamanho = st.st_size;

   printf("\t%d%%\n", progress);

   arq = fopen(arquivo, "r");
   if(arq == NULL){
      printf("\tERRO ao abrir arquivo\n");
   }

   for(int i = 0; i <= tamanho - tamanho%63; i+=63){
      tam_read = fread(buff+4, sizeof(unsigned char), 63, arq);
      buff[1] = tam_read;
      buff[2] = *seq;

      //printf("%d\n", buff[2]);
      clearLines();
      printf("\tEnviando... %.2f%%\n", (++progress/ (float) (tamanho/63)) * 100);
      
      send(socket, buff, 67, 0);
      (*seq) = ((*seq) + 1) % 256;
      usleep(1);
   }

   tam_read = fread(buff+4, sizeof(unsigned char), tamanho%63, arq);
   if(tam_read > 0){
      tam_read = fread(buff+4, sizeof(unsigned char), tamanho%63, arq);
      buff[1] = tam_read;
      buff[2] = *seq;
      
      //printf("%d\n", buff[2]);

      send(socket, buff, 67, 0);
      (*seq) = ((*seq) + 1) % 256;
      usleep(1);
   }

   fclose(arq);
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