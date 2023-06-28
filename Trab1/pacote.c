#include "utils.h"
#include <sys/time.h>

/**
 * @brief Timestamp
 * 
 * @return long long  Timestamp gerado
 */
long long timestamp(){
   struct timeval tp;
   gettimeofday(&tp, NULL);
   return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

/**
 * @brief Checa se uma mensagem é a anterior -> já foi recebida
 * 
 * @param servidor      Flag se é o servidor
 * @param seq           Estrutura de sequência
 * @param seq_recebida  Sequencia recebida (atual)
 * @return int          1 caso seja a anterior e 0 caso não
 */
int mensagem_anterior(int servidor, seq_t *seq, int seq_recebida, int tipo_recebido){
   
   if(tipo_recebido != T_NACK && tipo_recebido != T_ACK && tipo_recebido != T_OK){
      if (servidor){
         if(seq_recebida == 63 && seq->client == 0){
            return 1;
         }
         else if(seq_recebida == seq->client-1){
            return 1;
         }
         return 0;
      }
      else{
         if(seq_recebida == 63 && seq->server == 0){
            return 1;
         }
         else if(seq_recebida == seq->server-1){
            return 1;
         }
         return 0;
      }
   }

   return 0;
}


/**
 * @brief Construct a new calcula paridade object
 * 
 * @param buffer  buffer da mensagem
 * @param tam     tamanho da mensagem
 */
unsigned char calcula_paridade(unsigned char *buffer, int tam){
   unsigned char pari = buffer[1] ^ buffer[2];
   for (int i = 0; i < tam; ++i){
      pari = pari ^ buffer[i+3];
   }

   return pari;
}


/**
 * @brief Printa erro baseado no codigo
 * 
 * @param erro codigo do errno
 */
void print_erro(int erro){
   printf("\t");
   switch (erro){
      case ERRO_ARQ_NEXISTE:
         printf("ERRO: Arquivo não existe\n");
         break;

      case ERRO_DISCO_CHEIO:
         printf("ERRO: Disco cheio\n");
         break;

      case ERRO_PERM_ESCRITA:
         printf("ERRO: Sem permissão de escrita\n");
         break;

      case ERRO_PERM_LEITURA:
         printf("ERRO: Sem permissão de leitura\n");
         break;
      
      default:
         break;
   }
   return;
}


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
 * @brief Recebe do socket
 * 
 * @param socket        Socket a receber
 * @param buffer        Buffer de mensagem
 * @param tam_buffer    Tamanho do buffer
 * @return int          0 em sucesso ou -1 em caso de erro 
 */
int recebe(int socket, unsigned char *buffer, int tam_buffer){
   long long comeco = timestamp();
   int bytes_lidos;

   do{
      bytes_lidos = recv(socket, buffer, tam_buffer, 0);
      if(bytes_lidos > 0 && buffer[0] == 126){
         return bytes_lidos;
      }
   } while(timestamp() - comeco <= 5000);
   return -1;
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
int envia(int socket, unsigned char *dados, int tam, int tipo, seq_t *seq, int wait, int answer_t, unsigned char *buffReturn){
   pacote_t p, resposta;
   unsigned char buffer[67] = {0}, buffer_resposta[67] = {0};
   unsigned char bufferD[134] = {0}, buffer_respostaD[134] = {0};
   int ret;

   if(tam > 63){
      return -1;
   }

   p.ini  = 126;
   p.tam  = tam;
   if(seq){
      if(!servidor){
         p.seq  = seq->client;
      }
      else{
         p.seq  = seq->server;
      }
   }
   p.tipo = tipo;

   memcpy(buffer, &p, 3);
   if(dados){
      memcpy(buffer+3, dados, tam);
   }

   // Calcula paridade par
   buffer[66] = calcula_paridade(buffer, p.tam);

   int i = 0;
   int j = 0;
   while(i < 134){
      bufferD[i] = buffer[j];
      bufferD[i+1] = 0xFF;

      i+=2;
      j++;
   }

   ret = send(socket, bufferD, sizeof(bufferD), 0);

   if(wait){
      //Aguarda resposta
      while(1){
         if (recebe(socket, buffer_respostaD, sizeof(buffer_respostaD)) > 0){
            int i = 0;
            int j = 0;
            while(i < 134){
               buffer_resposta[i] = buffer_respostaD[j];

               i++;
               j+=2;
            }
            
            memcpy(&resposta, buffer_resposta, 3);
            if(resposta.ini == 126){
               // Caso paridade não bata, manda nack
               if(buffer_resposta[66] != calcula_paridade(buffer_resposta, resposta.tam)){
                  envia(socket, NULL, 0, T_NACK, NULL, 0, 0, NULL);
               }
               // Pegou mensagem já processada (Sequência já passou)
               else if ( mensagem_anterior(servidor, seq, resposta.seq, resposta.tipo) ){
                  envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
               }
               // Mensagem nova
               else{
                  if(resposta.tipo == answer_t){
                     if(buffReturn){
                        memcpy(buffReturn, buffer_resposta+3, resposta.tam);
                     }
                     break;
                  }
                  else if(resposta.tipo == T_ERRO){
                     char *ptr = (char *)buffer_resposta+3+resposta.tam;
                     int tipo = strtoul((char *)buffer_resposta+3, &ptr, 10);
            
                     print_erro(tipo);
                     ret = -1;
                     break;
                  }
                  //Caso receba NACK, manda novamente
                  else if(resposta.tipo == T_NACK){
                     ret = send(socket, bufferD, 134, 0);
                  }
               }
            }
         }
         //else if(!servidor){
         else{
            //Manda de novo
            printf("Timeout estourou! Tentando mandar novamente!\n");
            ret = send(socket, bufferD, 134, 0);
         }
      }
   }

   // Aumenta as sequencia de forma separada
   if(seq){
      if(!servidor){
         seq->client = (seq->client + 1) % 64;
      }
      else{
         seq->server = (seq->server + 1) % 64;
      }
   }

   return ret;
}


/**
 * @brief Recupera arquivo(s)
 * 
 * @param socket     Socket de rede
 * @param seq        Estrutura de sesquência
 * @param arquivo    Nome do arquivo, caso recupere um só
 * @param varios     Se recupera um arquivo só ou vários
 */
void recuperaArquivo(int socket, seq_t *seq, char* arquivo, int varios){
   char *ptr;
   int tipo, esc, fim = 0, cont = 0;
   unsigned char buffRecover[67], buffRecoverD[134];
   char filename[63];
   pacote_t packRecover;

   if(!varios){
      strcpy(filename, arquivo);
   }

   while(!fim){
      if(recv(socket, buffRecoverD, sizeof(buffRecoverD), 0) > 0){
         int i = 0;
         int j = 0;
         while(i < 134){
            buffRecover[i] = buffRecoverD[j];

            i++;
            j+=2;
         }

         memcpy(&packRecover, buffRecover, 3);
         // Caso paridade não bata, manda nack
         if(packRecover.ini == 126){
            if(buffRecover[66] != calcula_paridade(buffRecover, packRecover.tam)){
               envia(socket, NULL, 0, T_NACK, NULL, 0, 0, NULL);
            }
            // Pegou mensagem já processada (Sequência já passou)
            else if ( mensagem_anterior(servidor, seq, packRecover.seq, packRecover.tipo) ){
               envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
            }
            else if(packRecover.seq == seq->server){
               switch(packRecover.tipo){
                  case T_NOME_ARQ_REC:
                     strncpy(filename, (char*)buffRecover+3, packRecover.tam);
                     printf("\tRecebendo %s...\n", filename);
                     break;
                  
                  case T_DADOS:
                     esc = escreveParte(filename, buffRecover+3, packRecover.tam);
                     if (esc > 0){
                        print_erro(errno);
                     }
                     
                     envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
                     break;

                  case T_FIM_ARQUIVO:
                     printf("\t\tArquivo %s restaurado com sucesso\n", filename);
                     cont++;
                     if(!varios){
                        fim = 1;
                     }
                     break;

                  case T_FIM_GRUPO:
                     printf("\t%d arquivos restaurados com sucesso!\n", cont);
                     if(varios){
                        fim = 1;
                     }
                     break;
                  
                  case T_ERRO:
                     ptr = (char *)buffRecover+3+packRecover.tam;
                     tipo = strtoul((char *)buffRecover+3, &ptr, 10);
                     print_erro(tipo);
                     break;
                  
                  default:
                     break;
               }
            }
            seq->server = (seq->server+ 1) % 64;
         }
      }
   }
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

   for(int i = 0; i < tamanho - tamanho%63; i+=63){
      tam_read = fread(buff, sizeof(unsigned char), 63, arq);
      
      clearLines();
      printf("\tEnviando... %.2f%%\n", (++progress/tam_total) * 100);

      envia(socket, buff, tam_read, T_DADOS, seq, 1, T_ACK, NULL);
   }

   tam_read = fread(buff, sizeof(unsigned char), tamanho%63, arq);
   if(tam_read > 0){
      clearLines();
      printf("\tEnviando... %.2f%%\n", (++progress/tam_total) * 100);

      envia(socket, buff, tam_read, T_DADOS, seq, 1, T_ACK, NULL);
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
   int w = fwrite(dados, sizeof(unsigned char), tamanho, arq);
   if(w <= 0){
      return errno;
   }

   fclose(arq);
   return 0;
}

/**
 * @brief Escreve erro na tela e manda mensagem baseada nele
 * 
 * @param erro       Codigo do errno
 * @param socket     Socket da rede
 * @param seq        Sequencia
 */
void escreveErro_e_envia_pkgerro(char *filename, int erro, int socket, seq_t *seq)
{
   unsigned char ans[63];
   switch (erro){
      case ENOENT:
         printf("Arquivo não existe\n");
         sprintf((char *)ans, "%d", ERRO_ARQ_NEXISTE);
         envia(socket, ans, strlen((char *)ans)+1, T_ERRO, seq, 0, 0, NULL);
         break;
      
      case EACCES:
         printf("Permissão negada\n");

         if((access(filename, R_OK) == 0) && (access(filename, W_OK) != 0)){
            sprintf((char *)ans, "%d", ERRO_PERM_ESCRITA);
         }
         else{
            sprintf((char *)ans, "%d", ERRO_PERM_LEITURA);
         }
         envia(socket, ans, strlen((char *)ans)+1, T_ERRO, seq, 0, 0, NULL);
         break;

      case ENOSPC:
         printf("Sem espaço no disco\n");
         sprintf((char *)ans, "%d", ERRO_DISCO_CHEIO);
         envia(socket, ans, strlen((char *)ans)+1, T_ERRO, seq, 0, 0, NULL);
         break;
      
      default:
         break;
      }
}
           