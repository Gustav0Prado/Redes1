typedef struct  __attribute__((packed, scalar_storage_order("big-endian"))) {
   unsigned int ini  : 8;
   unsigned int tam  : 6;
   unsigned int seq  : 6;
   unsigned int tipo : 4;
} pacote_t;

void enviaArquivo(int socket, char *arquivo, int *seq);
void escreveParte(char *arquivo, unsigned char *dados, int tamanho);