all:			client server

common.o:		common.cpp common.h
	g++ -c common.cpp

client.o:		client.cpp client.h common.o
	g++ -c client.cpp

server.o:		server.cpp server.h common.o
	g++ -c server.cpp

gaosocket.o:	gaosocket.cpp gaosocket.h
	g++ -c gaosocket.cpp

client:			common.o client.o gaosocket.o
	g++ -o client common.o gaosocket.o client.o

server:			common.o server.o gaosocket.o
	g++ -o server common.o gaosocket.o server.o

clean:
	rm *.o *.gch client server
