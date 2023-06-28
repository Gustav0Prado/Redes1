#include "utils.h"

int servidor;

void escolheEntrada(int argc, char **argv){
   char lixo;
    if(argc == 1){
      printf("0 - Cliente\n1 - Servidor\n");
      scanf("%d", &servidor);
      scanf("%c", &lixo);
      system("clear");
   }
   else{
      if(strcmp(argv[1], "cliente") == 0){
         servidor = 0;
      }
      else if(strcmp(argv[1], "servidor") == 0){
         servidor = 1;
      }
      else{
         printf("Por favor selecione uma opção válida:\n");
         printf("0 - Cliente\n1 - Servidor\n");
         scanf("%d", &servidor);
         scanf("%c", &lixo);
         system("clear");
      }
   }

   if(servidor){
      system("clear");
      printf("Iniciado como servidor\n");
   }
   else{
      system("clear");
      printf("Iniciado como cliente\n");
      printf("Digite ver_comandos ou vc para os possíveis comandos\n");
   }
}

/*  Retorna tempo em milisegundos

    Forma de uso:
 
    double tempo;
    tempo = timestamp();
    <trecho de programa do qual se deseja medir tempo>
    tempo = timestamp() - tempo;
*/

// double timestamp(void)
// {
//   struct timespec tp;
//   clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
//   return((double)(tp.tv_sec*1.0e3 + tp.tv_nsec*1.0e-6));
// }