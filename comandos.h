#define CDLOCAL             0
#define BACKUP_UM           1
#define BACKUP_VARIOS       2

int codigoComando(char *token);
void cdLocal(char *caminho);
void backup1Arquivo(char *arquivo);
void backupVariosArquivo(char *regex);