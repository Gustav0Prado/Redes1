#define CDLOCAL             0
#define BACKUP_UM           1
#define BACKUP_VARIOS       2
#define LS                  3
#define MD5                 4
#define QUIT               99

int codigoComando(char *token);
void cdLocal(char *caminho);
void backup1Arquivo(int socket, char *arquivo, int *seq);
void backupVariosArquivos(char *expr);
int geraMD5(char *arquivo, unsigned char* c);