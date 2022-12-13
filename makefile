CC=gcc
FLAGS= -Wall -g

all: send recv dgramS dgramC

dgramS.o: dgramS.c
	$(CC) $(FLAGS) -c dgramS.c

dgramS: dgramS.o
	$(CC) $(FLAGS) -o dgramS dgramS.o

dgramC.o: dgramC.c
	$(CC) $(FLAGS) -c dgramC.c

dgramC: dgramC.o
	$(CC) $(FLAGS) -o dgramC dgramC.o

SendData.o: SendData.c
	$(CC) $(FLAGS) -c SendData.c -lpthread

send: SendData.o
	$(CC) $(FLAGS) -o send SendData.o 

recvData.o: recvData.c
	$(CC) $(FLAGS) -c recvData.c

recv: recvData.o
	$(CC) $(FLAGS) -o recv recvData.o 

.PHONY: clean all 

clean:
	rm -f *.o *.a *.so send recv dgramS dgramC