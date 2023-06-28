#include "utils.h"

/**
 * @brief Retorna codigo correspondente a string de um comando
 * 
 * @param token String com comando
 * @return int Codigo do comando
 */
int codigoComando(char *token){
   if(strcmp(token, "cdlocal") == 0)         return CDLOCAL;
   if(strcmp(token, "cdremoto") == 0)        return CDREMOTO;
   if( (strcmp(token, "backup_um") == 0)        || (strcmp(token, "bu") == 0) )        return BACKUP_UM;
   if( (strcmp(token, "backup_varios") == 0)    || (strcmp(token, "bv") == 0) )        return BACKUP_VARIOS;
   if( (strcmp(token, "restaura_um") == 0)      || (strcmp(token, "ru") == 0) )        return RESTAURA_UM;
   if( (strcmp(token, "restaura_varios") == 0)  || (strcmp(token, "rv") == 0) )        return RESTAURA_VARIOS;
   if( (strcmp(token, "ver_comandos") == 0)     || (strcmp(token, "vc") == 0) )        return VER_COMANDOS;
   if(strcmp(token, "ls") == 0)              return LS;
   if(strcmp(token, "md5") == 0)             return MD5;
   if(strcmp(token, "quit") == 0)            return QUIT;
   else return -1;
}

/**
 * @brief Muda para o diretorio indicado pelo caminho
 * 
 * @param caminho Caminho _ para o diretório desejado
 */
void cdLocal(char *caminho){
   if(caminho){
      if(chdir(caminho) == 0){
         printf("\tDiretorio mudado para %s\n", getcwd (NULL, 0));
      }
      else{
         printf("\tERRO ao mudar diretorio\n");
      }
   }
   else{
      printf("\tERRO ao ler caminho\n");
   }
}

/**
 * @brief Muda para o diretorio indicado pelo caminho
 * 
 * @param caminho Caminho _ para o diretório desejado
 */
void cdRemoto(int socket, char *caminho, seq_t *seq){
   if(caminho && strlen(caminho) <= 63){
      envia(socket, (unsigned char *)caminho, strlen(caminho)+1, T_CD_REMOTO, seq, 1, T_OK, NULL);
   }
   else{
      printf("\tERRO ao ler caminho\n");
   }
}

/**
 * @brief Envia o backup de um arquivo para o servidor
 * 
 * @param arquivo Caminho para o arquivo com o backup a ser feito
 */
void backup1Arquivo(int socket, char *arquivo, seq_t *seq){
   // Checa se arquivo existe
   if(access(arquivo, R_OK) == 0){
      envia(socket, (unsigned char *)arquivo, strlen(arquivo)+1, T_BACKUP_UM, seq, 1, T_OK, NULL);
      if(enviaArquivo(socket, arquivo, seq) == 0){
         envia(socket, NULL, 0, T_FIM_ARQUIVO, seq, 1, T_ACK, NULL);
      }
      
      else{
         printf("ERRO AO LER NOME DO ARQUIVO: NOME MUITO GRANDE\n");
      }
   }
   else{
      printf("Erro ao abrir arquivo: ");
      switch(errno){
         case ENOENT:
            printf("arquivo não existe\n");
            break;

         case EACCES:
            printf("arquivo sem permissão de leitura\n");
            break;
      }
   }
}

/**
 * @brief Envia 1 arquivo para o socket
 * 
 * @param socket     Socket a usar
 * @param arquivo    Nome do arquivo
 * @param seq        Sequencia
 */
void enviaVariosArquivos(int socket, char *expr, seq_t *seq){
   int i=0;
   glob_t globbuf;
   unsigned char qtd[1];

   if (glob(expr, 0, NULL, &globbuf) == 0) {
      printf("%ld arquivos a enviar\n", globbuf.gl_pathc);

      //Salva no buffer qtd de arquivos
      qtd[0] = globbuf.gl_pathc;
      if(!servidor) envia(socket, qtd, 1, T_BACKUP_VARIOS, seq, 1, T_OK, NULL);

      //Envia cada um deles
      for (i=0; i <globbuf.gl_pathc; i++) { 
         if(!servidor){
            envia(socket, (unsigned char *)globbuf.gl_pathv[i], strlen(globbuf.gl_pathv[i])+1, T_BACKUP_UM, seq, 1, T_OK, NULL);
            if(enviaArquivo(socket, globbuf.gl_pathv[i], seq) == 0){
               envia(socket, NULL, 0, T_FIM_ARQUIVO, seq, 1, T_ACK, NULL);
            }
         }
         else{
            envia(socket, (unsigned char *)globbuf.gl_pathv[i], strlen(globbuf.gl_pathv[i])+1, T_NOME_ARQ_REC, seq, 0, 0, NULL);
            if(enviaArquivo(socket, globbuf.gl_pathv[i], seq) == 0){
               envia(socket, NULL, 0, T_FIM_ARQUIVO, seq, 0, 0, NULL);
            }
         }
      }

      if(!servidor) envia(socket, NULL, 0, T_FIM_GRUPO, seq, 1, T_ACK, NULL);
      globfree(&globbuf);
   }
}


/**
 * @brief Restaura 1 arquivo recebido do socket
 * 
 * @param socket     Socket a usar
 * @param arquivo    Nome do arquivo
 * @param seq        Sequencia
 */
void restaura1Arquivo(int socket, char *arquivo, seq_t *seq){
   // unsigned char buffRecover[67], buffRecoverD[134];
   // pacote_t packRecover;
   char confirm, lixo;

   if (access(arquivo, 0) == 0){
      printf("Arquivo já existe, sobrescrever? (s/n) ");
      scanf("%c", &confirm);
      scanf("%c", &lixo);

      if(confirm == 'n'){
         printf("Operação cancelada\n");
         return;
      }
      else{
         remove(arquivo);
      }
   }

   int e = envia(socket, (unsigned char *)arquivo, strlen(arquivo)+1, T_RECUPERA_UM, seq, 0, 0, NULL);

   if(e > 0){
      recuperaArquivo(socket, seq, arquivo, 0);
      // char *ptr;
      // int tipo, esc;

      // int fim = 0;
      // while(!fim){
      //    if(recv(socket, buffRecoverD, sizeof(buffRecoverD), 0) > 0){
      //       int i = 0;
      //       int j = 0;
      //       while(i < 134){
      //          buffRecover[i] = buffRecoverD[j];

      //          i++;
      //          j+=2;
      //       }

      //       memcpy(&packRecover, buffRecover, 3);
      //       if(packRecover.ini == 126){
      //          // Caso paridade não bata, manda nack
      //          if(buffRecover[66] != calcula_paridade(buffRecover, packRecover.tam)){
      //             envia(socket, NULL, 0, T_NACK, NULL, 0, 0, NULL);
      //          }
      //          // Pegou mensagem já processada (Sequência já passou)
      //          else if ( mensagem_anterior(servidor, seq, packRecover.seq, packRecover.tipo) ){
      //             envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
      //          }
      //          else if(packRecover.seq == seq->server){
      //             switch(packRecover.tipo){
      //                case T_DADOS:
      //                   esc = escreveParte(arquivo, buffRecover+3, packRecover.tam);
      //                   if (esc > 0){
      //                      print_erro(errno);
      //                   }

      //                   envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
      //                   break;

      //                case T_FIM_ARQUIVO:
      //                   printf("\tArquivo %s restaurado com sucesso\n", arquivo);
      //                   fim = 1;
      //                   break;
                     
      //                case T_ERRO:
      //                   ptr = (char *)buffRecover+3+packRecover.tam;
      //                   tipo = strtoul((char *)buffRecover+3, &ptr, 10);
      //                   print_erro(tipo);
      //                   fim = 1;
      //                   break;
                     
      //                default:
      //                   break;
      //             }
      //             seq->server = (seq->server+ 1) % 64;
      //          }
      //       }
      //    }
      // }
   }
}

/**
 * @brief Envia 1 arquivo para o socket
 * 
 * @param socket     Socket a usar
 * @param arquivo    Nome do arquivo
 * @param seq        Sequencia
 */
void restauraVariosArquivos(int socket, char *expr, seq_t *seq){
   // int fim = 0, cont = 0;
   // unsigned char buffRecover[67], buffRecoverD[134];
   // char filename[63];
   // pacote_t packRecover;
   char confirm, lixo;

   printf("Sobrescrever todos os arquivos? (s/n) ");
   scanf("%c", &confirm);
   scanf("%c", &lixo);

   if(confirm == 'n'){
      printf("Operação cancelada\n");
      return;
   }

   envia(socket, (unsigned char *)expr, strlen(expr)+1, T_RECUPERA_VARIOS, seq, 0, 0, NULL);

   recuperaArquivo(socket, seq, NULL, 1);

   // char *ptr;
   // int tipo, esc;

   //Espera ate fim do grupo
   // while(!fim){
   //    if(recv(socket, buffRecoverD, sizeof(buffRecoverD), 0) > 0){
   //       int i = 0;
   //       int j = 0;
   //       while(i < 134){
   //          buffRecover[i] = buffRecoverD[j];

   //          i++;
   //          j+=2;
   //       }

   //       memcpy(&packRecover, buffRecover, 3);
   //       // Caso paridade não bata, manda nack
   //       if(packRecover.ini == 126){
   //          if(buffRecover[66] != calcula_paridade(buffRecover, packRecover.tam)){
   //             envia(socket, NULL, 0, T_NACK, NULL, 0, 0, NULL);
   //          }
   //          // Pegou mensagem já processada (Sequência já passou)
   //          else if ( mensagem_anterior(servidor, seq, packRecover.seq, packRecover.tipo) ){
   //             envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
   //          }
   //          else if(packRecover.seq == seq->server){
   //             switch(packRecover.tipo){
   //                case T_NOME_ARQ_REC:
   //                   strncpy(filename, (char*)buffRecover+3, packRecover.tam);
   //                   printf("\tRecebendo %s...\n", filename);
   //                   break;
                  
   //                case T_DADOS:
   //                   esc = escreveParte(filename, buffRecover+3, packRecover.tam);
   //                   if (esc > 0){
   //                      print_erro(errno);
   //                   }
                     
   //                   envia(socket, NULL, 0, T_ACK, NULL, 0, 0, NULL);
   //                   break;

   //                case T_FIM_ARQUIVO:
   //                   printf("\t\tArquivo %s restaurado com sucesso\n", filename);
   //                   cont++;
   //                   break;

   //                case T_FIM_GRUPO:
   //                   fim = 1;
   //                   printf("\t%d arquivos restaurados com sucesso!\n", cont);
   //                   break;
                  
   //                case T_ERRO:
   //                   ptr = (char *)buffRecover+3+packRecover.tam;
   //                   tipo = strtoul((char *)buffRecover+3, &ptr, 10);
   //                   print_erro(tipo);
   //                   break;
                  
   //                default:
   //                   break;
   //             }
   //          }
   //          seq->server = (seq->server+ 1) % 64;
   //       }
   //    }
   // }
   
}


/**
 * @brief Cria MD5 para um arquivo e o retorna no vetor c
 * 
 * @param arquivo Nome do arquivo
 * @param md5 Vetor de unsigned char com md5 gerado
 * 
 * @return int Tamanho da string md5
 */
int geraMD5(char *arquivo, unsigned char* md5){
   EVP_MD_CTX *mdctx;
   FILE *arq;
   struct stat st;
   unsigned char data[1024];
   unsigned int md5_digest_len = EVP_MD_size(EVP_md5());

   // Tenta abrir arquivo desejado
   arq = fopen(arquivo, "r");
   if (arq == NULL) {
      /* Retornar pacote com erro */
      printf ("\tArquivo %s não existe!\n", arquivo);
      return -1;
   }

   // Gera tamanho do arquivo
   stat(arquivo, &st);
   long tamanho = st.st_size;

   // MD5_Init
   mdctx = EVP_MD_CTX_new();
   EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

   // MD5_Update
   for(int i = 0; i < tamanho/1024; i++){
      fread(data, 1, 1024, arq);
      EVP_DigestUpdate(mdctx, data, 1024);
   }
   fread(data, 1, tamanho%1024, arq);
   EVP_DigestUpdate(mdctx, data, tamanho%1024);

   // MD5_Final
   unsigned char *md5_digest = (unsigned char *)OPENSSL_malloc(md5_digest_len);
   EVP_DigestFinal_ex(mdctx, md5_digest, &md5_digest_len);
   EVP_MD_CTX_free(mdctx);

   strcpy((char*)md5, (char*)md5_digest);
   free(md5_digest);

   fclose (arq);

   return md5_digest_len;
}

/**
 * @brief Verifica o md5 do arquivo local com o arquivo de backup
 * 
 * @param socket  Socket por onde envia mensagens
 * @param arquivo Nome do arquivo a ser verificado
 * @param seq     Estrutura de sequencia
 */
void checaMD5(int socket, char *arquivo, seq_t *seq){
   unsigned char md5Local[1024], md5Remoto[1024];

   int e = envia(socket, (unsigned char *)arquivo, strlen(arquivo)+1, T_VERIFICA_BACKUP, seq, 1, T_MD5, md5Remoto);

   seq->server = (seq->server + 1) % 64;

   if(e > 0){
      int len = geraMD5(arquivo, md5Local);

      //Printa MD5 na tela
      printf("MD5 local:  ");
      for(int i = 0; i < len; i++){
         printf("%02x", md5Local[i]);
      }
      printf("\n");

      //Printa MD5 na tela
      printf("MD5 remoto: ");
      for(int i = 0; i < len; i++){
         printf("%02x", md5Remoto[i]);
      }
      printf("\n");

      int s = strncmp((char *)md5Local, (char*)md5Remoto, 16);
      if(s == 0){
         printf("MD5 bate! Arquivos idênticos\n");
      }
      else{
         printf("MD5 não bate! Arquivos diferentes\n");
      }
   }
}