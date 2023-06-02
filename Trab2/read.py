#!/usr/bin/python3

import socket
import sys, termios
import random

ips = []
portas = []
bastao = 0
personalDeck = []
hostId = ips.index(socket.gethostname())


def init_deck (deck):
    for i in range (0, 13):
        for j in range (0, i):
            deck.append(i)

    #appending jesters
    deck.append(13)
    deck.append(13)
    random.shuffle(deck)

def the_deal(deck, playersNum):
    card = 0
    while (len(deck) > 0):
        for i in range (playersNum):
            if (len(deck) < 0):
                return
            card = deck.pop()
            if (i == hostId):
                personalDeck.append(card)
            else:
                send("()")

        card = deck.pop()

def check_confirm(message, playersNum):
    confirm = message[-2]
    


def send (message):
    s.sendto(message.encode(), (ips[ (hostId+1) % qtd  ], int(portas[ (hostId+1) % qtd ])) )
    rec_data, addr = listen.recvfrom(1024)
    check_confirm(rec_data)




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
print("Listening on ", portas[hostId])

# Envia pacotes para o próximo nó
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
print("Sending to", ips[ (hostId+1) % qtd ], "on port", portas[ (hostId+1) % qtd ])

# Primeira maquina do arquivo começa com o bastão
if(hostId == 0):
    bastao = True

while(True):
    # Se tiver o bastão, envia mensagem, espera receber de volta e passa o bastão pra frente
    if(bastao):
        termios.tcflush(sys.stdin, termios.TCIOFLUSH)
        send_data = input("\t>>> Você possui o bastao, envie uma mensagem: ")
        s.sendto(send_data.encode(), (ips[ (hostId+1) % qtd  ], int(portas[ (hostId+1) % qtd ])) )

        rec_data, addr = listen.recvfrom(1024)

        s.sendto(b'b', (ips[ (hostId+1) % qtd   ], int(portas[ (hostId+1) % qtd ])) )
        bastao = False
    # Se nao tiver o bastao espera uma mensagem
    else:
        print("\tVocê nao possui o bastao, aguarde uma mensagem")
        rec_data, addr = listen.recvfrom(1024)
        if(rec_data.decode() == 'b'):
           bastao = True
        else:
            print("Mensagem recebida: ", rec_data.decode())
            s.sendto(rec_data, (ips[ (hostId+1) % qtd   ], int(portas[ (hostId+1) % qtd ])) )

