#!/usr/bin/python3

import socket, random, os, sys, termios

ips = []
portas = []
personalDeck = []
passControler = []
playersFinished = []
bastao = False
dealing = True
iFinished = False
hostId = 0
nextHost = 0
last_played_card = 1000
last_played_qtd = 1000
last_player = -1
jester = 13

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
   print("descartar x y: você descarta as cartas que deseja na sua rodada (x cartas y)")
   print("descartar x y + z 13: você pode descartar as cartas que deseja + um número de coringas")
   print("ver_deck: você imprime o deck que está na sua mão")




def print_personalDeck(deck):
    print(f"você possui {len(deck)} cartas" )
    print("seu deck é composto por:")
    for i in range(1, 14):
      if(deck.count(i) > 0):
         if(i <= 9 ):
               print(str(i) + "  : " + str(deck.count(i)))
         else:
               print(str(i) + " : "  + str(deck.count(i)))
   
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


def discard(play_qtd, play_card, sender, listener, playersNum, jester_qtd = 0):
   """Descarta play_qtd cartas do tipo play_card da sua mão

   Args:
       play_qtd (int): Quantidade descartada
       play_card (int): Carta descartada
       sender (socket): Socket para enviar
       listener (socket): Socket para ouvir
       playersNum (int): Quantidade de jogadores
   """
   global iFinished
   global playersFinished
   global bastao
   global last_played_card
   global last_played_qtd
   global last_player

   # Caso haja cartas suficientes, remove play_card por play_qtd vezes
   if((personalDeck.count(play_card) >= play_qtd                                       #caso haja cartas suficientes
      and ((play_qtd+jester_qtd == last_played_qtd) or (last_played_qtd == 1000))      #quantidade de cartas deve ser igual
      and play_card < last_played_card                                                 #caso as cartas sejam de maior prioridade  >>>> falta numero de cartas
      and personalDeck.count(jester) >= jester_qtd)):
      #remove as cartas play_card por play_qtd vezes
      for i in range(play_qtd):
         for elem in personalDeck:
            if(elem == play_card):
               del(personalDeck[personalDeck.index(elem)])
               break
      #se remover coringa, remove coringa por jester_qtd vezes
      if(jester_qtd > 0):
         for i in range(jester_qtd):
            for elem in personalDeck:
               if(elem == jester):
                  del(personalDeck[personalDeck.index(elem)])
                  break         
            
      print ("Mão atual: ", personalDeck)

      #se ainda houver cartas
      if(len(personalDeck) > 0):
         send(f"({hostId}hd{play_qtd:02d}{play_card:02d}{jester_qtd}000000000)", playersNum, sender, listener)
      #caso acabou o deck -> este jogador terminou
      else:
         send(f"({hostId}hd{play_qtd:02d}{play_card:02d}{jester_qtd}100000000)", playersNum, sender, listener)
         iFinished = True
         playersFinished.append(int(hostId))

         os.system("clear")
         print(f"Terminou o jogo! {len(playersFinished)}° lugar")

      send(f"({hostId}tp00000000)", playersNum, sender, listener)
      bastao = False
      last_player = hostId
   else:
      print ("Jogada Inválida!")
      if(not(personalDeck.count(play_card) >= play_qtd)):
          print ("cartas insuficientes no baralho")
      if(not(play_card < last_played_card)):
          print ("última carta jogada é menor")
      if(not(personalDeck.count(jester) >= jester_qtd)):
          print ("coringas insuficientes no baralho")
      print("última carta jogada foi " + str(last_played_card))


         
           
             

   
def nextRound(playersNum, sender, listener):
   """Passa mensagem para limpar telas
   """
   global last_played_card
   global last_played_qtd
   global last_player

   os.system("clear")
   print ("Rodada terminou, reseta nível de descarte")
   last_played_card = 1000
   last_played_qtd = 1000
   last_player = -1

   if bastao:
      send(f"({hostId}nr00000000)", playersNum, sender, listener)


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
   
   # Se tiver bastao, mensagem roda o anel.
   # Se for token pass, só passa pra frente sem esperar voltar
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
   global playersFinished
   global last_played_card
   global last_played_qtd
   global last_player

   rec_data, addr = listener.recvfrom(1024)
   rec_data = rec_data.decode()

   #rec_data - [0]inicio; [1]origem; [2:4]tipo; [4:-9]jogada; [-9:-1]confirmação; [1]fim; 
   rec_msg = Mensagem(rec_data[0], rec_data[1], rec_data[2:4], rec_data[4:-9], rec_data[-9:-1], rec_data[-1])

   # Caso ja tenha terminado, apenas repassa mensagens
   if iFinished:
      if(rec_msg.tipo == "hd" and rec_msg.jogada[-1] == "1"):
         playersFinished.append(int(rec_msg.origem))
      if(rec_msg.tipo == "tp"):
         bastao = True
         if last_player == hostId:
            nextRound(playersNum, sender, listener)
         bastao = False

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
            # Printa com tabs :)
            for i in range(int(rec_msg.origem)):
               print("\t", end="")

            #se não tiver descartado coringas
            if(rec_msg.jogada[4] == "0"):
               print(f"Jogador {rec_msg.origem} descartou {int(rec_msg.jogada[:2])} carta(s) {int(rec_msg.jogada[2:4])}")
            #se tiver descartado coringas
            else:
               print(f"Jogador {rec_msg.origem} descartou {int(rec_msg.jogada[:2])} carta(s) {int(rec_msg.jogada[2:4])} e {int(rec_msg.jogada[4])} coringas")

            #Atualiza utlima carta e ultimo jogador
            last_played_qtd  = int(rec_msg.jogada[:2]) + int(rec_msg.jogada[4])
            last_played_card = int(rec_msg.jogada[2:4])
            last_player      = int(rec_msg.origem)


            #se recebe o bit de confirmação mais significativo como 1, o jogador passado terminou o jogo
            if(rec_msg.jogada[5] == "1"):
               for i in range(int(rec_msg.origem)):
                  print("\t", end="")
               print (f"Jogador {rec_msg.origem} terminou!")
               playersFinished.append(int(rec_msg.origem))

            rec_msg.confirmacao = flip_bit(rec_msg.confirmacao, hostId)
            send(str(rec_msg), playersNum, sender, listener)

         # simple pass
         elif(rec_msg.tipo == "sp"):
            #Print com tabs :)
            for i in range(int(rec_msg.origem)):
               print("\t", end="")
            print(f"Jogador {rec_msg.origem} passou o turno!")
            passControler[int(rec_msg.origem)] = 1
         
         
            # Confirma recebimento e passa pra frente
            rec_msg.confirmacao = flip_bit(rec_msg.confirmacao, hostId)
            send(str(rec_msg), playersNum, sender, listener)

         # next round
         elif(rec_msg.tipo == "nr"):
            nextRound(playersNum, sender, listener)

            # Confirma recebimento e passa pra frente
            rec_msg.confirmacao = flip_bit(rec_msg.confirmacao, hostId)
            send(str(rec_msg), playersNum, sender, listener)


def main():
   global hostId
   global nextHost
   global bastao
   global personalDeck
   global playersFinished
   global iFinished
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
   #print(hostId)

   nextHost = (hostId+1) % playersNum
   
   #inicia controladores do jogo
   for i in range (playersNum):
      passControler.append(0) #controlador de quem passou o turno 
   

   # Escuta pacotes vindos do nó anterior na porta atual
   listen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   listen.bind( (socket.gethostbyname(ips[hostId]), int(portas[hostId])) )
   #print("Listening on ", portas[hostId])

   # Envia pacotes para o próximo nó
   s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   #print("Sending to", ips[nextHost], "on port", portas[nextHost])

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
   print_personalDeck(personalDeck)
   partida = True

   #partida acaba quando todos os jogadores tiverem mãos vazias
   while(partida):
      # Caso já tenha terminado ou não tenha o bastão, apenas repassa mensagens
      if iFinished or (not bastao):
         receive(s, listen, playersNum)

      elif (bastao):
         if (last_player == hostId):
            nextRound(playersNum, s, listen)

         print("_________________________________________________")
         print(f"Jogador {hostId}, é sua vez! qual sua ação? Digite ver_comandos")
         print (">>>", end="")

         jogada = input()
         if len(jogada) > 0:
            jogada = jogada.split()
            if  ((jogada[0] == "ver_comandos") or (jogada[0] == "vc")):
               ver_comandos()

            elif((jogada[0] == "passo") or (jogada[0] == "p")):
               pass_turn(playersNum, s, listen)

            elif((jogada[0] == "ver_deck") or (jogada [0] == "vd")):
               print_personalDeck(personalDeck)

            elif((jogada[0] == "descartar") or (jogada[0] == "d")):
               if(len(jogada) == 1):
                  #caso não tenha digitado a carta e a quantidade a descartar, solicita
                  print("Descartar _ _: ", end="")
                  disc = input()
                  disc = disc.split()
                  if(len(disc) == 2):
                     discard(int(disc[0]), int(disc[1]), s, listen, playersNum)
                  if(len(disc) == 5):
                     discard(int(disc[0]), int(disc[1]), s, listen, playersNum, int(disc[3]))

               elif(len(jogada) == 3):
                  discard(int(jogada[1]), int(jogada[2]), s, listen, playersNum)

               elif(len(jogada) == 6):
                  discard(int(jogada[1]), int(jogada[2]), s, listen, playersNum, int(jogada[4]))
            else:
               print("Erro na jogada!")

      if len(playersFinished) == playersNum-1:
         partida = False
         if hostId not in playersFinished:
            playersFinished.append(hostId)

   # Fim do jogo
   os.system("sl")

   #Reescreve configuracao de acordo com ranking
   if os.path.exists("conf.txt"):
      os.remove("conf.txt")
   with open("conf.txt", "x") as f:
      f.write(f"{playersNum}\n")
      for player in playersFinished:
         f.write(f"{ips[player]} {portas[player]}\n")
   

if __name__ == "__main__":
      main()
