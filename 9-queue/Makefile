CC = gcc

all: server client

server:
	$(CC) -pthread ExtraQ_server.c -o server.out

client:
	$(CC) -pthread ExtraQ_client.c -o client.out

clean:
	rm -rf *\.out *\.o
