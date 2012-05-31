#
# Example memcache+tpl Build Environment
#

CC=gcc
CP=cp
CFLAGS=-Wall -g -I. -ltpl -lmemcached
VERSION=1.0

example: example.o
	$(CC) $(CFLAGS) -o example example.o

clean:
	rm -rf *.o example struct.tpl
