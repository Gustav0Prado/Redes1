#!/usr/bin/python3

import socket

class Maquina:
   def __init__(self, ip, porta):
      self.ip = ip
      self.porta = porta
   
   def proximaMaq(self):

conexoes = []
with open("conf.txt") as f:
   qtdMaquinas = int(f.readline())
   while True:
      line = f.readline()
      if not line:
         break

      entrada = line.split()
      ip = entrada[0]
      porta = entrada[1]

      conexoes.append(ip)
      conexoes.append(porta)
   print(conexoes)

IpAtual = socket.gethostbyname(socket.gethostname())