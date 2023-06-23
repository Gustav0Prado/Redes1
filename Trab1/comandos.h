#include "pacote.h"

#define CDLOCAL             0
#define BACKUP_UM           1
#define BACKUP_VARIOS       2
#define LS                  3
#define MD5                 4
#define CDREMOTO            5
#define QUIT               99


int codigoComando(char *token);
void cdLocal(char *caminho);
void cdRemoto(int socket, char *caminho, seq_t *seq);
void backup1Arquivo(int socket, char *arquivo, seq_t *seq);
void backupVariosArquivos(char *expr);
int geraMD5(char *arquivo, unsigned char* c);