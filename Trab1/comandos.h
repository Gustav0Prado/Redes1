#define CDLOCAL             0
#define BACKUP_UM           1
#define BACKUP_VARIOS       2
#define LS                  3

int codigoComando(char *token);
void cdLocal(char *caminho);
void backup1Arquivo(int socket, char *arquivo);
void backupVariosArquivos(char *expr);