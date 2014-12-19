server:server.o battleship.o
	gcc -o server server.o -lpthread
	gcc -o battleship battleship.o -lpthread

server.o:server.c
	gcc -c server.c -o server.o

battleship.o:battleship.c
	gcc -c battleship.c -o battleship.o


clean:
	rm *.o server battleship