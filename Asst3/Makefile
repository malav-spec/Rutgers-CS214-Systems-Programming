all: client server

client: client.c 
	gcc client.c -o WTF -lcrypto

server: server.c
	gcc server.c -o WTFserver
	
clean:
	rm -f WTF WTFserver
