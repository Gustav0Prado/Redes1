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
void backup1Arquivo(char *arquivo){
   // Checa se arquivo existe
   if(access(arquivo, R_OK) == 0){
      //envia mensagem de inicio com o nome do arquivo
      //espera ok
      enviaArquivo(arquivo);
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
void backupVariosArquivo(char *regex){
   /*
      Acessa diretorio
      Para cada arquivo
         Checa se primeira parte satisfaz regex
         Se sim, checa se extensao satisfaz regex
               Se sim, backup1Arquivo
         Passa pro proximo
   */
   char expr[64];
   FILE *arq;

   printf("Digite o nome do arquivo desejado: ");
   fgets(expr, 64, stdin);
   expr[strlen(expr)-1] = '\0';


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
   MD5_CTX mdContext;
   FILE *arq;
   struct stat st;
   char arquivo[64];
   unsigned char data[1024];

   // Tenta abrir arquivo desejado
   arq = fopen(arquivo, "r");
   if (arq == NULL) {
      /* Retornar pacote com erro */
      printf ("Arquivo %s não existe!\n", arquivo);
      return 0;
   }

   // Gera tamanho do arquivo
   stat(arquivo, &st);
   long tamanho = st.st_size;

   //Inicia MD5
   MD5_Init (&mdContext);
   
   // Lê arquivo e atualiza md5 para cada bloco lido
   for(int i = 0; i < tamanho - tamanho%63; i+=63){
      fread(data, 1, 63, arq);
      MD5_Update (&mdContext, data, 63);
   }
   fread(data, 1, tamanho%63, arq);
   MD5_Update (&mdContext, data, tamanho%63);

   //finaliza md5
   MD5_Final (c,&mdContext);

   //Printa MD5 na tela
   // for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
   //    printf("%02x", c[i]);
   // }
   // printf("\n");

   fclose (arq);
}