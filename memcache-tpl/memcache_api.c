/*
********************************************************************************
*
* Module: memcache_api.c
*
* Purpose: Unit Tests for Memcache and TPL serialization wrapper.
*
* APIs:
*
* Notes:
*
* To run unit tests -
*
 gcc -c -g -I. -D_MAIN_ -I../include memcache_api.c
 gcc -g -I. -ltpl -lmemcached -o memcache_api memcache_api.o
*
* ./memcache_api
*
********************************************************************************
*/


/* Includes *******************************************************************/
#include "memcache_api.h"

#ifdef _MAIN_
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


typedef struct person {
    char *first_name;
    char *last_name;
    int age;
} PERSON;
#define _PERSON_MAP "S(ssi)"


int main(int argc, char *argv[])
{

	const char *personLogFMT = "%s\n----------------------------\nfirst_name\t= %s\nlast_name\t= %s\nage\t\t= %d\n\n";
	const char *rewardLogFMT = "";

	PERSON customer = {"Brian\000", "McManus\000", 72};
	PERSON expanded = {"\000", "\000", 0};

	fprintf(stdout, personLogFMT, "BEFORE SERIALIZATION", customer.first_name, customer.last_name, customer.age);
	MEMCACHE_INIT("192.168.10.170", "192.168.10.169");

	//Simple serialization and storage for the structure above.
	MEMCACHE_STORE("12345667", "customer", _PERSON_MAP, &customer);
	MEMCACHE_GET("12345667", "customer", _PERSON_MAP, &expanded);
	fprintf(stdout, personLogFMT, "DE-SERIALIZATION", expanded.first_name, expanded.last_name, expanded.age);


	//Clean-up Variables, Free Memory.
	MEMCACHE_DESTROY();
}
#endif
