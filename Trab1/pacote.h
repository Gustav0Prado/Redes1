#pragma once

typedef struct  __attribute__((packed, scalar_storage_order("big-endian"))) {
   unsigned int ini  : 8;
   unsigned int tam  : 6;
   unsigned int seq  : 6;
   unsigned int tipo : 4;
} pacote_t;

typedef struct seq_t
{
    unsigned int client:6;
    unsigned int server:6;
}seq_t;

int enviaArquivo(int socket, char *arquivo, seq_t *seq);
int escreveParte(char *arquivo, unsigned char *dados, int tamanho);
int envia(int socket, unsigned char *dados, int tam, int tipo, seq_t *seq, int wait, int answer_t, unsigned char *buffReturn);
void print_erro(int erro);
unsigned char calcula_paridade(unsigned char *buffer, int tam);
int mensagem_anterior(int servidor, seq_t *seq, int seq_recebida, int tipo_recebido);
int recebe(int socket, unsigned char *buffer, int tam_buffer);