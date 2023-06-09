#!/usr/bin/python3

import socket, random, os

ips = []
portas = []
personalDeck = []
bastao = False
hostId = 0
nextHost = 0
finished = 0
iFinished = False

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
   # for i in range (0, 13):
   #    for j in range (0, i):
   #       deck.append(i)

   #appending jesters
   deck.append(13)
   deck.append(13)
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

         if (i == hostId):
            personalDeck[card] += 1
         else:
            # cd == card deal
            send(f"({hostId}cd{i}{card}/0)", playersNum, sender, listener)


def check_confirm(message, playersNum):
   """Checa se mensagem foi recebida corretamente por todos do anel

   Args:
      message (string): Mensagem recebida a ser checada
      playersNum (int): Quantidade de jogadores na partida

   Returns:
      bool: True caso correto ou False caso haja erro
   """
   index = message.find('/')+1 
   confirm = int(message[index:-1])
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
   val_bin = bin(value)[2:].zfill(8)
   if (val_bin[n] == '0'):
      return value | (1 << n)
   else:
      return value & ~(1 << n)
    


def send (message, playersNum, sender, listener):
   """Envia mensagem pelo anel até receber de volta e confirmar o recebimento

   Args:
       message (string): Mensagem a ser enviada
       playersNum (int): Número de jogadores
       sender (socket): Socket de envio
       listener (socket): Socket local de recebimento
   """
   check = False
   i = 0
   # Se tiver bastao, mensagem roda o anel. S for token pass, so passa pra frente se esperar voltar
   if bastao:
      if(message[2:4] == "tp"):
         sender.sendto(message.encode(), (ips[nextHost], int(portas[nextHost])) )
      else:
         while(check == False and i < 100):
            sender.sendto(message.encode(), (ips[nextHost], int(portas[nextHost])) )
            rec_data, addr = listener.recvfrom(1024)
            rec_data = rec_data.decode()

            # flipa 'bit' de confirmacao e cria mensagem auxiliar
            confirmation = flip_bit(rec_data[-9:-1], hostId)
            rec_aux = rec_data[:-9] + confirmation + rec_data[-1] 

            check = check_confirm(rec_aux, playersNum)
            if not check:
               print("Mensagem não recebida, mandando novamente")
            i += 1
         # não confirmou recebimento da mensagem
         assert i < 100
   # sem bastao, só repassa a mensagem
   else:
      sender.sendto(message.encode(), (ips[ (hostId+1) % playersNum  ], int(portas[ (hostId+1) % playersNum ])) )


def receive (sender, listener, playersNum):
   global bastao
   global dealing
   global finished

   rec_data, addr = listener.recvfrom(1024)
   rec_data = rec_data.decode()
   index = rec_data.find('/')+1
   rec_msg = Mensagem(rec_data[0], rec_data[1], rec_data[2:4], rec_data[4:index], int(rec_data[index:-1]), rec_data[-1])

   # Caso ja tenha terminado, apenas repassa mensagens
   if iFinished:
      if(rec_msg.tipo == "hd" and rec_msg.jogada[4] == "1"):
         finished += 1

      rec_msg.confirmacao = flip_bit(rec_msg.confirmacao, hostId)
      sender.sendto(str(rec_msg).encode(), (ips[nextHost], int(portas[nextHost])) )

   else:
      if(rec_msg.inicio == '('  and  rec_msg.fim == ')'):
            # token pass
            if(rec_msg.tipo   == "tp"):
               bastao = True
            
            # card deal
            elif(rec_msg.tipo == "cd"):
               if(int(rec_msg.jogada[0]) == hostId):
                  personalDeck[int(rec_msg.jogada[1:])] += 1

               # Confirma recebimento e passa pra frente
               rec_msg.confirmacao = flip_bit(rec_msg.confirmacao, hostId)
               send(str(rec_msg), playersNum, sender, listener)
            
            # end deal
            elif(rec_msg.tipo == "ed"):
               dealing = False
               rec_msg.confirmacao = flip_bit(rec_msg.confirmacao, hostId)
               send(str(rec_msg), playersNum, sender, listener)

            # hand discard
            elif(rec_msg.tipo == "hd"):
               for i in range(int(rec_msg.origem)):
                  print("\t", end="")
               print(f"Jogador {rec_msg.origem} descartou {int(rec_msg.jogada[:2])} cartas {int(rec_msg.jogada[2:4])}")

               if(rec_msg.jogada[4] == "1"):
                  for i in range(int(rec_msg.origem)):
                     print("\t", end="")
                  print (f"Jogador {rec_msg.origem} terminou!")
                  finished += 1

               rec_msg.confirmacao = flip_bit(rec_msg.confirmacao, hostId)
               send(str(rec_msg), playersNum, sender, listener)

            # simple pass
            elif(rec_msg.tipo == "sp"):
               print ("faz sp")


def print_deck(deck):
   """Printa um deck (de forma legível)

   Args:
       deck (list):Deck em formato de lista
   """
   showDeck = []
   for i in range(13):
      showDeck.append(i)

   print ("Sua mão é: ")

   for i in range(int(sum(deck)*1.65)):
      print("-", end="")
   print ("")

   for i in range(13):
      if(i == 0):
         print(f"| Ás ", end="")
      elif(i == 12):
         print(f"|  J ", end="")
      else:
         print(f"| {showDeck[i]+1:02d} ", end="")

   print ("|")
   for i in range(66):
      print("-", end="")

   print ("")
   for i in range(1, 14):
      print(f"| {deck[i]:02d} ", end="")
   
   #print(f"| - {sum(deck)} cartas")

   print ("|")
   for i in range(int(sum(deck)*1.65)):
      print("-", end="")
   print ("")

def main():
   global hostId
   global nextHost
   global bastao
   global personalDeck
   global finished
   global iFinished

   # Le arquivo de configuracao e coloca nomes das maquinas e portas em listas
   with open("conf.txt") as f:
      playersNum = int(f.readline())
      while True:
         line = f.readline()
         if not line:
            break

         entrada = line.split()
         ips.append(entrada[0])
         portas.append(entrada[1])

   hostId = ips.index(socket.gethostname())
   #print(hostId)

   # Escuta pacotes vindos do nó anterior na porta atual
   listen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   listen.bind( (socket.gethostbyname(ips[hostId]), int(portas[hostId])) )
   #print("Listening on ", portas[hostId])

   # Envia pacotes para o próximo nó
   s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   #print("Sending to", ips[nextHost], "on port", portas[nextHost])

   for i in range(14):
      personalDeck.append(0)

   # Primeira maquina do arquivo começa com o bastão
   global bastao
   if(hostId == 0):
      bastao = True
      print("Dando Cartas...\n")
      deck = []
      init_deck(deck)
      the_deal(deck, playersNum, s, listen)
   else:
      if (hostId < 80 % playersNum):
         valorWhile =  (80 // playersNum) + 1
      else:
         valorWhile =  80//playersNum
      while(len(personalDeck) < valorWhile):
         receive(s, listen, playersNum)
   
   print_deck(personalDeck)
   
   while(finished < playersNum):
      if iFinished or (not bastao):
         receive(s, listen, playersNum)
      elif bastao:
         print (">>>", end="")
         play = input()
         play = play.split(' ')

         play_qtd  = int(play[0])
         play_card = int(play[1])

         if(personalDeck[play_card] >= play_qtd):
            personalDeck[play_card] -= play_qtd

            os.system("clear")
            print_deck(personalDeck)

            if(sum(personalDeck) > 0):
               send(f"({hostId}hd{play_qtd:02d}{play_card:02d}000000000)", playersNum, s, listen)
            else:
               send(f"({hostId}hd{play_qtd:02d}{play_card:02d}100000000)", playersNum, s, listen)
               iFinished = True
               finished += 1
               print(f"Terminou o jogo! {finished}° lugar")

            send(f"({hostId}tp00000000)", playersNum, s, listen)
            bastao = False
         else:
            print ("Jogada Inválida!")


if __name__ == "__main__":
      main()
