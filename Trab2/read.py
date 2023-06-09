#!/usr/bin/python3

import socket
import random

ips = []
portas = []
personalDeck = []
bastao = False
dealing = True
hostId = 0
nextHost = 0
passControler = []

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

def ver_comandos():
   print("As possíveis jogadas são: ")
   print("passo : você passa o seu turno para o próximo jogador sem descartar")
   print("descartar: você descarta as cartas que deseja na sua rodada")
   print("ver_deck: você imprime o deck que está na sua mão")




def print_personalDeck(deck):
    print(f"você possui {len(deck)} cartas" )
    print("seu deck é composto por:")
    for i in range(1, 14):
      if(i <= 9 ):
         print(str(i) + "  :" + str(deck.count(i)))
      else:
         print(str(i) + " :" + str(deck.count(i)))
   
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

def pass_turn(playersNum, sender, listener):
   """Manda uma mensagem para os outros jogadores que o turno foi passado
   
   Args:

      playersNum (int): Quantidade de jogadores na partida
      sender: 
      listener:
   """
   global bastao
   #turn pass
   send(f"({hostId}spp00000000)", playersNum, sender, listener)

   #token pass
   bastao = False
   send(f"({hostId}tpp00000000)", playersNum, sender, listener)


   

def the_deal(deck, playersNum, sender, listener):
   """Cartear baralho

   Args:
      deck (list): Lista com cartas do baralho já embaralhadas
      playersNum (int): Quantidade de jogadores na partida
      sender:
      listener
   """
   card = 0
   while (len(deck) > 0):
      for i in range (playersNum):
         if (len(deck) <= 0):
            break
   
         card = deck.pop()

         if (i == hostId):
            personalDeck.append(card)
         else:
            # cd == card deal
            send(f"({hostId}cd{i}{card}00000000)", playersNum, sender, listener)
   # Avisa que acabaram as cartas
   send(f"({hostId}ed00000000)", playersNum, sender, listener)


def check_confirm(message, playersNum):
   """Checa se mensagem foi recebida corretamente por todos do anel

   Args:
      message (string): Mensagem recebida a ser checada
      playersNum (int): Quantidade de jogadores na partida

   Returns:
      bool: True caso correto ou False caso haja erro
   """
   confirm = message[-9:-1]
   if (confirm.count("1") != playersNum):
      return False
   return True
    


def flip_bit(value, n):
   """Flipa o n-ésimo bit (da direita pra esquerda) de value

   Args:
      value (string): Valor em bits
      n (int): Posição do bit a ser flipado

   Returns:
      int: Valor com bit já flipado
   """
   if (value[n] == '0'):
      return value[:n] + '1' + value[n+1:]
   else:
      return value[:n] + '0' + value[n+1:]
    


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
   # Se tiver bastao, mensagem roda o anel

   if bastao:
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
      sender.sendto(message.encode(), (ips[nextHost], int(portas[nextHost])) )

def receive (sender, listener, playersNum):
   """Aguarda ate receber uma mensagem e a processa

   Args:
       sender (socket): socket para enviar
       listener (socket): socket para ouvir
       playersNum (int): Quantidade de jogadores
   """
   global bastao
   global dealing

   rec_data, addr = listener.recvfrom(1024)
   rec_data = rec_data.decode()
   #rec_data - [0]inicio; [1]origem; [2:4]tipo; [4:-9]jogada; [-9:-1]confirmação; [1]fim; 
   rec_msg = Mensagem(rec_data[0], rec_data[1], rec_data[2:4], rec_data[4:-9], rec_data[-9:-1], rec_data[-1])

   if(rec_msg.inicio == '('  and  rec_msg.fim == ')'):
      # token pass
      if(rec_msg.tipo   == "tp"):
         bastao = True
      
      # card deal
      elif(rec_msg.tipo == "cd"):
         if(int(rec_msg.jogada[0]) == hostId):
            personalDeck.append(int(rec_msg.jogada[1:]))

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
         print ("faz hd")

      # simple pass
      elif(rec_msg.tipo == "sp"):
         print(f"Jogador {rec_msg.origem} passou o turno!")
         passControler[int(rec_msg.origem)] = 1
        
        
         # Confirma recebimento e passa pra frente
         rec_msg.confirmacao = flip_bit(rec_msg.confirmacao, hostId)
         send(str(rec_msg), playersNum, sender, listener)         




def main():
   global hostId
   global nextHost
   global bastao
   global passControler

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
   print(hostId)

   nextHost = (hostId+1) % playersNum
   
   #inicia controladores do jogo
   for i in range (playersNum):
      passControler.append(0) #controlador de quem passou o turno 
   

   # Escuta pacotes vindos do nó anterior na porta atual
   listen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   listen.bind( (socket.gethostbyname(ips[hostId]), int(portas[hostId])) )
   print("Listening on ", portas[hostId])

   # Envia pacotes para o próximo nó
   s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   print("Sending to", ips[nextHost], "on port", portas[nextHost])

   # Primeira maquina do arquivo começa com o bastão e dá as cartas
   if(hostId == 0):
      bastao = True
      print("Dealing Cards")
      deck = []
      init_deck(deck)
      the_deal(deck, playersNum, s, listen)
   else:
      while(dealing):
         receive(s, listen, playersNum)
   
   personalDeck.sort()
   print(f"{personalDeck} - {len(personalDeck)}")
   partida = True
   while(partida):#partida acaba quando todos os jogadores tiverem mãos vazias
      if (bastao):
         print("_____________________________________________")
         print(f"Jogador {hostId}, é sua vez! qual sua ação? Digite ver_comandos")
         jogada = input()
         if  (jogada == "ver_comandos"):
            ver_comandos()
         elif(jogada == "passo"):
            pass_turn(playersNum, s, listen)
         elif(jogada == "ver_deck"):
            print_personalDeck()
         elif(jogada == "descartar"):
            print("Em construção")
      else:
         receive(s,listen, playersNum)
if __name__ == "__main__":
      main()
