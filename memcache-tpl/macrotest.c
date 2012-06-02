#include "memcache_api.h"
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
	const char *logfmt = "%s\n----------------------------\nfirst_name\t= %s\nlast_name\t= %s\nage\t\t= %d\n\n";
	PERSON customer = {"Brian\000", "McManus\000", 72};
	PERSON expanded = {"\000", "\000", 0};

	fprintf(stdout, logfmt, "BEFORE SERIALIZATION", customer.first_name, customer.last_name, customer.age);

	MEMCACHE_INIT("192.168.10.170", "192.168.10.169");
	MEMCACHE_STORE("12345667", "customer", _PERSON_MAP, &customer);
	MEMCACHE_GET("12345667", "customer", _PERSON_MAP, &expanded);

	fprintf(stdout, logfmt, "DE-SERIALIZATION", expanded.first_name, expanded.last_name, expanded.age);

	MEMCACHE_DESTROY();
}
