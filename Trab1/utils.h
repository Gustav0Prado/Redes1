#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <glob.h>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <errno.h>
#include "comandos.h"
#include "pacote.h"

#define ERRO_DISCO_CHEIO  0
#define ERRO_PERM_ESCRITA 1
#define ERRO_ARQ_NEXISTE  2
#define ERRO_PERM_LEITURA 3
