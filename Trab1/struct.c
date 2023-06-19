#include <stdio.h>
#include <string.h>

typedef struct  __attribute__((packed, scalar_storage_order("big-endian"))) {
   unsigned int ini  : 8;
   unsigned int tam  : 6;
   unsigned int seq  : 6;
   unsigned int tipo : 4;
} mystruct_A;

int main(){
   mystruct_A a, b;
   unsigned char m[3];

   a.ini  = 126;
   a.tam  = 2;
   a.seq  = 33;
   a.tipo = 3;

   // Copia struct para unsigned char
   memcpy(m, &a, 3);

   // Printa bits enviados
   printf("0: %d - " , m[0]);
   for (int i = 0; i < 8; i++) {
      printf("%d", !!((m[0] << i) & 0x80));
   }
   printf("\n");

   printf("1: %d  - " , m[1]);
   for (int i = 0; i < 8; i++) {
      printf("%d", !!((m[1] << i) & 0x80));
   }
   printf("\n");

   printf("2: %d  - " , m[2]);
   for (int i = 0; i < 8; i++) {
      printf("%d", !!((m[2] << i) & 0x80));
   }
   printf("\n");


   // Copia unsigned char de volta pra struct
   memcpy(&b, m, 3);

   // Printa struct copiada
   printf("b = %d %d %d %d\n", b.ini, b.tam, b.seq, b.tipo);

   // Calcula paridade de tam + seq + tipo
   unsigned int pari = m[1] ^ m[2];
   printf("Paridade: ");
   for (int i = 0; i < 8; i++) {
      printf("%d", !!((~pari << i) & 0x80));
   }
   printf("\n");
}