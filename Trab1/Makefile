# ip -o -4 route show to default | awk '{print $5}'

all: main

CFLAGS = -Wall -lcrypto -g
OBJECTS = utils.o comandos.o pacote.o socket.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

main: $(OBJECTS) main.c
	gcc -o main main.c $(OBJECTS) $(CFLAGS)


clean:
	rm -rf ./*.o ./main
