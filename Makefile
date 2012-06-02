#
# Example memcache+tpl Build Environment
#

CC=gcc
CP=cp
CFLAGS=-Wall -g -I. -D_MAIN_ -ltpl -lmemcached
VERSION=1.0

all: example macrotest memcache_api

example: example.o
	$(CC) $(CFLAGS) -o example example.o

macrotest: macrotest.o
	$(CC) $(CFLAGS) -o macrotest macrotest.o

memcache_api: memcache_api.o
	$(CC) $(CFLAGS) -o memcache_api memcache_api.o

clean:
	rm -rf *.o example macrotest struct.tpl memcache_api
