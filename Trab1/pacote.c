#include "utils.h"

/**
 * @brief Limpa linha anterior do terminal
 * 
 */
void clearLines(){
   printf("\033[1A");
   for (int i = 0; i < 100; ++i){
      printf("\b");
   }
}

/**
 * @brief Envia uma mensagem pela rede
 * 
 * @param socket        Socket por onde enviar
 * @param dados         Buffer com os dados a serem enviados
 * @param tam           Tamanho dos dados (não do buffer)
 * @param tipo          Tipo da mensagem
 * @param seq           Sequência
 * @param wait          Se vai esperar resposta ou não
 * @param answer_t      Se esperar, qual tipo de resposta espera
 * @param ignore_time   Se ignora o timeout ou reenvia
 * @return int          Tamanho enviado ou -1 caso não consiga enviar
 */
int envia(int socket, unsigned char *dados, int tam, int tipo, seq_t *seq, int wait, int answer_t, int ignore_time){
   pacote_t p, resposta;
   unsigned char buffer[67], buffer_resposta[67];
   int s;

   p.ini  = 126;
   p.tam  = tam;
   p.seq  = (*seq).client;
   p.tipo = tipo;

   memcpy(buffer, &p, 3);
   if(dados){
      memcpy(buffer+4, dados, tam);
   }
   //memcpy(buffer+tam+3, &p+3, 1); -> paridade
   s = send(socket, buffer, 67, 0);
   
   if(wait){
      //Aguarda resposta
      while(1){
         if (recv(socket, buffer_resposta, sizeof(buffer_resposta), 0) > 0){
            memcpy(&resposta, buffer_resposta, 3);
            if(resposta.ini == 126 && resposta.tipo == answer_t){
               break;
            }
         }
         else if(!ignore_time){
            //Manda de novo
            s = send(socket, buffer, 67, 0);
         }
      }
   }

   ((*seq).client) = (((*seq).client) + 1) % 64;
   return s;
}




/**
 * @brief Divide e envia o arquivo indicado
 * 
 * @param arquivo Caminho para o arquivo a ser enviado
 */
void enviaArquivo(int socket, char *arquivo, seq_t *seq){
   FILE *arq;
   struct stat st;
   unsigned char buff[63];
   int tam_read, progress = 0;

   stat(arquivo, &st);
   long tamanho = st.st_size;

   //Envia nome do arquivo
   if(strlen(arquivo) <= 63){
      envia(socket, (unsigned char *)arquivo, strlen(arquivo)+1, T_BACKUP_UM, seq, 1, T_OK, 0);

      // Começa a mandar arquivo
      printf("\t%d%%\n", progress);

      arq = fopen(arquivo, "r");
      if(arq == NULL){
         printf("\tERRO ao abrir arquivo\n");
      }

      for(int i = 0; i <= tamanho - tamanho%63; i+=63){
         tam_read = fread(buff, sizeof(unsigned char), 63, arq);

         clearLines();
         printf("\tEnviando... %.2f%%\n", (progress++/ (float) (tamanho/63)) * 100);

         envia(socket, buff, tam_read, T_DADOS, seq, 1, T_ACK, 0);
      }

      tam_read = fread(buff+4, sizeof(unsigned char), tamanho%63, arq);
      if(tam_read > 0){
         tam_read = fread(buff, sizeof(unsigned char), tamanho%63, arq);

         clearLines();
         printf("\tEnviando... %.2f%%\n", (progress++/ (float) (tamanho/63)) * 100);

         envia(socket, buff, tam_read, T_DADOS, seq, 1, T_ACK, 0);
      }

      fclose(arq);
   }
   else{
      printf("ERRO: NOME DO ARQUIVO MUITO GRANDE\n");
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