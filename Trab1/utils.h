#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <glob.h>
#include <openssl/evp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <errno.h>
#include "comandos.h"
#include "pacote.h"
#include "socket.h"

#define ERRO_DISCO_CHEIO  0
#define ERRO_PERM_ESCRITA 1
#define ERRO_ARQ_NEXISTE  2
#define ERRO_PERM_LEITURA 3

#define T_BACKUP_UM        0
#define T_BACKUP_VARIOS    1
#define T_RECUPERA_UM      2
#define T_RECUPERA_VARIOS  3
#define T_CD_REMOTO        4
#define T_VERIFICA_BACKUP  5
#define T_NOME_ARQ_REC     6
#define T_MD5              7
#define T_DADOS            8
#define T_FIM_ARQUIVO      9
#define T_FIM_GRUPO        10
#define T_NADA             11
#define T_ERRO             12
#define T_OK               13
#define T_ACK              14
#define T_NACK             15

extern int servidor;

void escolheEntrada(int argc, char **argv);