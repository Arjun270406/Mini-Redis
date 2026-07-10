CC = gcc
CFLAGS = -Wall -pthread

all: mini_redis

mini_redis: server.c hashtable.c
	$(CC) $(CFLAGS) -o mini_redis server.c hashtable.c

clean:
	rm -f mini_redis
