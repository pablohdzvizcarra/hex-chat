client:
	gcc -o client src/client/client.c

server:
	gcc -o server src/server/server.c

clean:
	rm client server