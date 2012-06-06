#
# Example memcache+tpl Build Environment
#

CC=gcc
CP=cp
CFLAGS=-g -I. -D_MAIN_ -lrt -ltpl -lmemcached
VERSION=1.0

all: example memcache_api

example: example.o
	$(CC) $(CFLAGS) -o example example.o

#macrotest: macrotest.o
#	$(CC) $(CFLAGS) -o macrotest macrotest.o

memcache_api: memcache_api.o
	$(CC) $(CFLAGS) -o memcache_api memcache_api.o

clean:
	rm -rf *.o example macrotest struct.tpl memcache_api
