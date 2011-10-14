all:			client

common.o:		common.cpp common.h
	g++ -c common.cpp

client.o:		client.cpp client.h
	g++ -c client.cpp

gaosocket.o:	gaosocket.cpp gaosocket.h
	g++ -c gaosocket.cpp

client:			client.o gaosocket.o common.o
	g++ -o client client.o gaosocket.o common.o

clean:
	rm *.o client
