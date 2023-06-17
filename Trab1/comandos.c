#include "utils.h"

/**
 * @brief Retorna codigo correspondente a string de um comando
 * 
 * @param token String com comando
 * @return int Codigo do comando
 */
int codigoComando(char *token){
   if(strcmp(token, "cdlocal") == 0)       return CDLOCAL;
   if(strcmp(token, "backup_um") == 0)     return BACKUP_UM;
   if(strcmp(token, "backup_varios") == 0) return BACKUP_VARIOS;
   if(strcmp(token, "ls") == 0)            return LS;
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
 * @brief Envia o backup de um arquivo para o servidor
 * 
 * @param arquivo Caminho para o arquivo com o backup a ser feito
 */
void backup1Arquivo(int socket, char *arquivo, int *seq){
   // Checa se arquivo existe
   if(access(arquivo, R_OK) == 0){
      //envia mensagem de inicio com o nome do arquivo
      //espera ok
      enviaArquivo(socket, arquivo, seq);
      //envia fim
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
 * @brief Realiza backup de varios arquivos dada uma expressão regular
 * 
 * @param regex Expressão Regular dos arquivos desejados
 */
void backupVariosArquivos(char *expr){
   /*
      Acessa diretorio
      Para cada arquivo
         Checa se primeira parte satisfaz regex
         Se sim, checa se extensao satisfaz regex
               Se sim, backup1Arquivo
         Passa pro proximo
   */
   //FILE *arq;

   int i=0;
   glob_t globbuf;

   if (glob(expr, 0, NULL, &globbuf) == 0) {
      for (i=0; i <globbuf.gl_pathc; i++) { 
         printf("%s\n",globbuf.gl_pathv[i]);
      }
      globfree(&globbuf);
   }
}

/**
 * @brief Cria MD5 para um arquivo e o retorna no vetor c
 * 
 * @param arquivo Nome do arquivo
 * @param c Vetor de unsigned char com md5 gerado --> PRECISA TER TAMANHO MD5_DIGEST_LENGTH <--
 */
void geraMD5(char *arquivo, unsigned char* c){
   EVP_MD_CTX *mdctx;
   unsigned char *md5_digest;
   FILE *arq;
   struct stat st;
   unsigned char data[1024];
   unsigned int md5_digest_len = EVP_MD_size(EVP_md5());

   // Tenta abrir arquivo desejado
   arq = fopen(arquivo, "r");
   if (arq == NULL) {
      /* Retornar pacote com erro */
      printf ("Arquivo %s não existe!\n", arquivo);
      return;
   }

   // Gera tamanho do arquivo
   stat(arquivo, &st);
   long tamanho = st.st_size;

   // MD5_Init
   mdctx = EVP_MD_CTX_new();
   EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

   // MD5_Update
   for(int i = 0; i < tamanho - tamanho%63; i+=63){
      fread(data, 1, 63, arq);
      EVP_DigestUpdate(mdctx, data, 63);
   }

   // MD5_Final
   md5_digest = (unsigned char *)OPENSSL_malloc(md5_digest_len);
   EVP_DigestFinal_ex(mdctx, md5_digest, &md5_digest_len);
   EVP_MD_CTX_free(mdctx);


   //Printa MD5 na tela
   // for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
   //    printf("%02x", c[i]);
   // }
   // printf("\n");

   fclose (arq);
}