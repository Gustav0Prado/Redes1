#include "pacote.h"

#define CDLOCAL             0
#define BACKUP_UM           1
#define BACKUP_VARIOS       2
#define LS                  3
#define MD5                 4
#define CDREMOTO            5
#define RESTAURA_UM         6
#define RESTAURA_VARIOS     7
#define VER_COMANDOS        8
#define MD5_VARIOS          9
#define QUIT               99


int codigoComando(char *token);

void cdLocal(char *caminho);
void cdRemoto(int socket, char *caminho, seq_t *seq);

void backup1Arquivo(int socket, char *arquivo, seq_t *seq);
void enviaVariosArquivos(int socket, char *expr, seq_t *seq);

void restaura1Arquivo(int socket, char *arquivo, seq_t *seq);
void restauraVariosArquivos(int socket, char *expr, seq_t *seq);

int geraMD5(char *arquivo, unsigned char* c);
void checaMD5(int socket, char *arquivo, seq_t *seq);
void checaMD5Varios(int socket, char *expr, seq_t *seq);