typedef struct pacote{
    unsigned char marcador, paridade;
    unsigned char tamanho : 6;      // 6 Bits de tamanho
    unsigned char sequencia : 6;    // 6 bits de tamanho
    unsigned char tipo : 4;         // 4 bits de tipo
} pacote_t;