typedef struct pacote{
    unsigned char marcador, paridade;   // 8 bits de marcador e paridade
    unsigned char tamanho : 6;          // 6 Bits de tamanho
    unsigned char sequencia : 6;        // 6 bits de tamanho
    unsigned char tipo : 4;             // 4 bits de tipo
} pacote_t;

void enviaArquivo(int socket, char *arquivo);
void escreveParte(char *arquivo, unsigned char *dados, int tamanho);