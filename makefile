CC=gcc
FLAGS= -Wall -g

# all: dgram stream tcp udp pipe mmap shared
all: dgram stream pipe

run: all
	./dgram
	./stream
	./pipe


# UDS-Dgram socket
uds_dgram.o: uds_dgram.c
	$(CC) $(FLAGS) -c uds_dgram.c

dgram: uds_dgram.o
	$(CC) $(FLAGS) -o dgram uds_dgram.o
# _______________________________________


# UDS-Stream socket
uds_stream.o: uds_stream.c
	$(CC) $(FLAGS) -c uds_stream.c

stream: uds_stream.o
	$(CC) $(FLAGS) -o stream uds_stream.o
# _______________________________________


# TCP/IPv4 socket
tcp.o: tcp.c
	$(CC) $(FLAGS) -c tcp.c

tcp: tcp.o
	$(CC) $(FLAGS) -o tcp tcp.o
# _______________________________________


# UDP/IPv6 socket
udp.o: udp.c
	$(CC) $(FLAGS) -c udp.c

udp: udp.o
	$(CC) $(FLAGS) -o udp udp.o
# _______________________________________


# MMAP
mmap.o: mmap.c
	$(CC) $(FLAGS) -c mmap.c

mmap: mmap.o
	$(CC) $(FLAGS) -o mmap mmap.o
# _______________________________________


# PIPE
pipe.o: pipe.c
	$(CC) $(FLAGS) -c pipe.c

pipe: pipe.o
	$(CC) $(FLAGS) -o pipe pipe.o
# _______________________________________


# Shared memory between threads
shared_memory.o: shared_memory.c
	$(CC) $(FLAGS) -c shared_memory.c

shared: shared_memory.o
	$(CC) $(FLAGS) -o shared_memory shared_memory.o
# _______________________________________



.PHONY: clean all 

clean:
	rm -f *.o *.a *.so  bigfile.txt server.txt dgram stream tcp udp pipe mmap shared