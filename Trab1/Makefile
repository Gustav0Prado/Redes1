# ip -o -4 route show to default | awk '{print $5}'

all : main

CFLAGS = -Wall -g -lcrypto

main: socket.o comandos.o pacote.o main.c
	gcc -o main main.c socket.o comandos.o pacote.o $(CFLAGS)

socket.o: socket.c
	gcc -c socket.c $(CFLAGS)

comandos.o: comandos.c
	gcc -c comandos.c $(CFLAGS)

pacote.h: pacote.c
	gcc -c pacote.c $(CFLAGS)

clean:
	rm -rf ./*.o ./main
