#!/usr/bin/python3

import socket, random
import sys, termios

ips = []
portas = []
personalDeck = []
bastao = 0
hostId = ips.index(socket.gethostname())


def init_deck (deck):
   """Inicia baralho

   Args:
      deck (list): Lista onde o baralho será iniciado
   """
   for i in range (0, 13):
      for j in range (0, i):
         deck.append(i)

   #appending jesters
   deck.append(13)
   deck.append(13)
   random.shuffle(deck)


def the_deal(deck, playersNum):
   """Cartear baralho

   Args:
      deck (list): Lista com cartas do baralho já embaralhadas
      playersNum (int): Quantidade de jogadores na partida
   """
   card = 0
   i = 0
   while (len(deck) > 0):
      card = deck.pop()
      if(i == 0):
         personalDeck.append(card)
      #else:
         #send(cria_mensagem(str(i)+"c,"+str(card)))
      i = (i + 1) % playersNum


def check_confirm(message, playersNum):
   """Checa se mensagem foi recebida corretamente por todos do anel

   Args:
      message (string): Mensagem recebida a ser checada
      playersNum (int): Quantidade de jogadores na partida

   Returns:
      Bool: True caso correto ou False caso haja erro
   """
   
   confirm = message[-2]
   if (bin(confirm).count("1") != playersNum):
      return False
   return True
    


def flip_bit(value, n):
   """Flipa o n-ésimo bit (da direita pra esquerda) de value

   Args:
      value (int): Valor inteiro
      n (int): Posição do bit a ser flipado

   Returns:
      int: Valor com bit já flipado
   """

   if (bin(value)[n+2] == '0'):
      return value | (1 << n)
   else:
      return value & ~(1 << n)
   


def receive (message):
   """Recebe mensagem e marca campo de confirmação como recebido

   Args:
       message (string): Mensagem recebida
   """
   
   rec_data, addr = listen.recvfrom(1024)
   message = flip_bit(rec_data[-2], hostId)



def send (message):
   """Envia mensagem pela rede. Caso alguém do anel tenha tido erro, reenvia a mensagem

   Args:
      message (string): Mensagem a ser enviada
   """
   received = ''
   s.sendto(message.encode(), (ips[ (hostId+1) % qtd  ], int(portas[ (hostId+1) % qtd ])) )
   receive(received)
   if (not (check_confirm(received))) :
      send(message)


def pass_on(message):
   """Repassa mensagem para o próximo do anel

   Args:
       message (string): Mensagem a ser repassada
   """
   s.sendto(message, (ips[ (hostId+1) % qtd   ], int(portas[ (hostId+1) % qtd ])) )


def passa_bastao():
   """Passa o bastão para a próxima máquina
   """
   #send(cria_mensagem("b"))
   bastao = False


def process(message):
   """Processa a mensagem recebida

   Args:
      message (string): Mensagem a ser processada
   """

   if(message.decode()[2] == 'b'):
      bastao = True
   elif(message.decode()[2] == 'p'):
      #jogador passou a vez
      pass_on(received)
   elif(message.decode()[2] == 'j'):
      #processa jogada
      pass_on(received)

# Le arquivo de configuracao e coloca nomes das maquinas e portas em listas
with open("conf.txt") as f:
   qtd = int(f.readline())
   while True:
      line = f.readline()
      if not line:
         break

      entrada = line.split()
      ips.append(entrada[0])
      portas.append(entrada[1])

hostId = ips.index(socket.gethostname())

# Escuta pacotes vindos do nó anterior na porta atual
listen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
listen.bind( (socket.gethostbyname(ips[hostId]), int(portas[hostId])) )

# Envia pacotes para o próximo nó
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Primeira maquina do arquivo começa com o bastão
if(hostId == 0):
   bastao = True

while(True):
   # Se tiver o bastão, envia mensagem, espera receber de volta e passa o bastão pra frente
   if(bastao):
      termios.tcflush(sys.stdin, termios.TCIOFLUSH)
      send_data = input("\t>>> Você possui o bastao, envie uma mensagem: ")
      # s.sendto(send_data.encode(), (ips[ (hostId+1) % qtd  ], int(portas[ (hostId+1) % qtd ])) )

      # rec_data, addr = listen.recvfrom(1024)

      # s.sendto(b'b', (ips[ (hostId+1) % qtd   ], int(portas[ (hostId+1) % qtd ])) )
      # bastao = False
      send(send_data.encode())
      passa_bastao()
   # Se nao tiver o bastao espera uma mensagem
   else:
      # print("\tVocê nao possui o bastao, aguarde uma mensagem")
      # rec_data, addr = listen.recvfrom(1024)
      # if(rec_data.decode() == 'b'):
      #    bastao = True
      # else:
      #    print("Mensagem recebida: ", rec_data.decode())
      #    s.sendto(rec_data, (ips[ (hostId+1) % qtd   ], int(portas[ (hostId+1) % qtd ])) )
      received = ''
      receive(received)
      process(received)
      

