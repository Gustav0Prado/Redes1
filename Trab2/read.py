#!/usr/bin/python3

conexoes = {}
with open("conf.txt") as f:
   qtdMaquinas = f.readline()
   while True:
      line = f.readline()
      if not line:
         break
      entrada = line.split()
      ip = entrada[0]
      porta = entrada[1]
      conexoes[ip] = porta
   print(conexoes)

mAtual = 
