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
int envia(int socket, unsigned char *dados, int tam, int tipo, seq_t *seq, int wait, int answer_t){
   pacote_t p, resposta;
   unsigned char buffer[67] = {0}, buffer_resposta[67] = {0};
   int s;

   if(tam > 63){
      return -1;
   }

   p.ini  = 126;
   p.tam  = tam;
   if(seq){
      if(!servidor){
         p.seq  = (*seq).client;
      }
      else{
         p.seq  = (*seq).server;
      }
   }
   p.tipo = tipo;

   memcpy(buffer, &p, 3);
   if(dados){
      memcpy(buffer+4, dados, tam);
   }
   //memcpy(buffer+tam+3, &p+3, 1); -> paridade
   s = send(socket, buffer, sizeof(buffer), 0);

   if(wait){
      //Aguarda resposta
      while(1){
         if (recv(socket, buffer_resposta, sizeof(buffer_resposta), 0) > 0){
            memcpy(&resposta, buffer_resposta, 3);
            if(resposta.ini == 126 && resposta.tipo == answer_t){
               break;
            }
            //Manda NACK caso mensagem chegue incorreta
            else if (resposta.tipo != T_NACK){
               p.tipo = T_NACK;
               memcpy(buffer, &p, 3);
               send(socket, buffer, 67, 0);
            }
            //Caso receba NACK, manda novamente
            else{
               s = send(socket, buffer, 67, 0);
            }
         }
         else if(!servidor && errno == ETIME){
            //Manda de novo
            printf("Timeout estourou! Mandando novamente!\n");
            s = send(socket, buffer, 67, 0);
         }
      }
   }

   // Aumenta as sequencia de forma separada
   if(seq){
      if(!servidor){
         ((*seq).client) = (((*seq).client) + 1) % 64;
      }
      else{
         ((*seq).server) = (((*seq).server) + 1) % 64;
      }
   }
   return s;
}




/**
 * @brief Divide e envia o arquivo indicado
 * 
 * @param arquivo Caminho para o arquivo a ser enviado
 */
int enviaArquivo(int socket, char *arquivo, seq_t *seq){
   FILE *arq = NULL;
   struct stat st;
   unsigned char buff[63];
   int tam_read;
   float progress = 0;

   if (stat(arquivo, &st) < 0){
      return -1;
   }
   long tamanho = st.st_size;
   float tam_total = (tamanho/63 + (tamanho%63 != 0));

   // Começa a mandar arquivo
   printf("\t%f%%\n", progress);

   arq = fopen(arquivo, "r");
   if(arq == NULL){
      printf("\tERRO ao abrir arquivo\n");
      return -1;
   }

   for(int i = 0; i <= tamanho - tamanho%63; i+=63){
      tam_read = fread(buff, sizeof(unsigned char), 63, arq);

      clearLines();
      printf("\tEnviando... %.2f%%\n", (++progress/tam_total) * 100);

      envia(socket, buff, tam_read, T_DADOS, seq, 1, T_ACK);
   }

   tam_read = fread(buff+4, sizeof(unsigned char), tamanho%63, arq);
   if(tam_read > 0){
      tam_read = fread(buff, sizeof(unsigned char), tamanho%63, arq);

      clearLines();
      printf("\tEnviando... %.2f%%\n", (++progress/tam_total) * 100);

      envia(socket, buff, tam_read, T_DADOS, seq, 1, T_ACK);
   }

   fclose(arq);
   return 0;
}

/**
 * @brief Escreve partes de um arquivo
 * 
 * @param arquivo Caminho para o arquivo
 * @param dados Vetor com dados do pacote
 * @param tamanho Tamanho dos dados
 */
int escreveParte(char *arquivo, unsigned char *dados, int tamanho){
   FILE* arq;

   // Cria novo arquivo em modo append, para acumular os dados dos pacotes
   arq = fopen(arquivo, "a+");
   if(arq == NULL){
      printf("Erro ao abrir arquivo: ");
      return errno;
   }

   // Escreve dados no arquivo
   fwrite(dados, sizeof(unsigned char), tamanho, arq);

   fclose(arq);
   return 0;
}