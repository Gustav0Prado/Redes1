#!/usr/bin/python3

import socket
import sys, termios
import random

ips = []
portas = []
personalDeck = []
bastao = False
hostId = 0

class Mensagem:
  def __init__(self, inicio, origem, tipo, jogada, confirmacao, fim):
      self.inicio = inicio
      self.origem = origem
      self.tipo = tipo
      self.jogada = jogada
      self.confirmacao = confirmacao
      self.fim = fim
      return
  def __str__(self):
     return self.inicio + str(self.origem) + self.tipo + str(self.jogada) + str(self.confirmacao) + self.fim


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


def the_deal(deck, playersNum, sender, listener):
   """Cartear baralho

   Args:
      deck (list): Lista com cartas do baralho já embaralhadas
      playersNum (int): Quantidade de jogadores na partida
   """
   card = 0
   while (len(deck) > 0):
      for i in range (playersNum):
         if (len(deck) < 0):
            return
         
         card = deck.pop()
         print(f"Card = {card}")

         if (i == hostId):
            print(f"Appended card {card}")
            personalDeck.append(card)
         else:
            # cd == card deal
            send(f"({hostId}cd{i}{card}0)", playersNum, sender, listener)
            print(f"Sent card to {i}")


def check_confirm(message, playersNum):
   """Checa se mensagem foi recebida corretamente por todos do anel

   Args:
      message (string): Mensagem recebida a ser checada
      playersNum (int): Quantidade de jogadores na partida

   Returns:
      bool: True caso correto ou False caso haja erro
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
    


def send (message, playerNum, sender, listener):
   """Envia mensagem pelo anel até receber de volta e confirmar o recebimento

   Args:
       message (string): Mensagem a ser enviada
       playerNum (int): Número de jogadores
       sender (socket): Socket de envio
       listener (socket): Socket local de recebimento
   """
   check = False
   i = 0
   # Se tiver bastao, mensagem roda o anel

   print (bastao)
   if bastao:
      while(check == False and i < 100):
         sender.sendto(message.encode(), (ips[ (hostId+1) % playerNum  ], int(portas[ (hostId+1) % playerNum ])) )
         rec_data, addr = listener.recvfrom(1024)
         flip_bit(int(rec_data[-2]), hostId)
         check = check_confirm(rec_data)
         if not check:
            flip_bit(int(rec_data[-2]), hostId)
            print("Mensagem não recebida, mandando novamente")
         i += 1
      # não confirmou recebimento da mensagem
      assert i < 100
   # sem bastao, só repassa a mensagem
   else:
      sender.sendto(message.encode(), (ips[ (hostId+1) % playerNum  ], int(portas[ (hostId+1) % playerNum ])) )

def receive (sender, listener, playersNum):
   global bastao

   rec_data, addr = listener.recvfrom(1024)
   rec_data = rec_data.decode()
   rec_msg = Mensagem(rec_data[0], rec_data[1], rec_data[2:3], rec_data[4:-2], int(rec_data[-2]), rec_data[-1])

   if(rec_msg.inicio == '('  and  rec_msg.fim == ')'):
      # token pass
      if(rec_msg.tipo   == "tp"):
         bastao = True
      # card deal
      elif(rec_msg.tipo == "cd"):
         if(rec_msg.jogada[0] == hostId):
            print(f"Received card {rec_msg.jogada[1:-1]}")
            personalDeck.append(rec_msg.jogada[1:-1])

         flip_bit(rec_msg.confirmacao, hostId)
         send(str(rec_msg).encode(), playersNum, sender, listener)
      # hand discard
      elif(rec_msg.tipo == "hd"):


         flip_bit(rec_msg.confirmacao, hostId)
         send(str(rec_msg).encode(), playersNum, sender, listener)
      # simple pass
      elif(rec_msg.tipo == "sp"):


         flip_bit(rec_msg.confirmacao, hostId)
         send(str(rec_msg).encode(), playersNum, sender, listener)

def main():
   global hostId
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
   print(hostId)

   # Escuta pacotes vindos do nó anterior na porta atual
   listen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   listen.bind( (socket.gethostbyname(ips[hostId]), int(portas[hostId])) )
   print("Listening on ", portas[hostId])

   # Envia pacotes para o próximo nó
   s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   print("Sending to", ips[ (hostId+1) % qtd ], "on port", portas[ (hostId+1) % qtd ])

   # Primeira maquina do arquivo começa com o bastão
   global bastao
   if(hostId == 0):
      bastao = True

   #while(True):
      # # Se tiver o bastão, envia mensagem, espera receber de volta e passa o bastão pra frente
      # if(bastao):
      #    termios.tcflush(sys.stdin, termios.TCIOFLUSH)
      #    send_data = input("\t>>> Você possui o bastao, envie uma mensagem: ")
      #    s.sendto(send_data.encode(), (ips[ (hostId+1) % qtd  ], int(portas[ (hostId+1) % qtd ])) )

      #    rec_data, addr = listen.recvfrom(1024)

      #    s.sendto(b'b', (ips[ (hostId+1) % qtd   ], int(portas[ (hostId+1) % qtd ])) )
      #    bastao = False
      # # Se nao tiver o bastao espera uma mensagem
      # else:
      #    print("\tVocê nao possui o bastao, aguarde uma mensagem")
      #    rec_data, addr = listen.recvfrom(1024)
      #    if(rec_data.decode() == 'b'):
      #       bastao = True
      #    else:
      #          print("Mensagem recebida: ", rec_data.decode())
      #          s.sendto(rec_data, (ips[ (hostId+1) % qtd   ], int(portas[ (hostId+1) % qtd ])) )
   
   
   if(hostId == 0):
      print("Dealing Cards")
      deck = []
      init_deck(deck)
      the_deal(deck, qtd, s, listen)
   else:
      while(len(personalDeck) < 40):
         receive(s, listen, qtd)
         print("Received Card")
   
   print(len(personalDeck))
   print(personalDeck)

if __name__ == "__main__":
        main()
