#include "example.h"
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void commandline(char *argv[])
{
	fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
	exit(1);
}

void error(char *error, ...)
{
	va_list argp;
	va_start(argp, error);
	vfprintf(stderr, error, argp);
	va_end(argp);
	fprintf(stderr, "\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	const char *logfmt = "%s\n----------------------------\nfirst_name\t= %s\nlast_name\t= %s\nage\t= %d\n\n";
	const char *config_string= "--SERVER=192.168.10.170";
	PERSON customer = {"Brian\000", "McManus\000", 72};
	PERSON expanded = {"\000", "\000", 0};
	char *memcache_value = 0x00;
	size_t memcache_value_len = 0;
	uint32_t memcache_value_flags = 0;
	memcached_return *memcache_error = 0x00;
	tpl_node *tn = 0x00;
	void *addr = 0x00;
	size_t len;

	fprintf(stdout, logfmt, "BEFORE SERIALIZATION", customer.first_name, customer.last_name, customer.age);

	//TPL Serialization
	tn = tpl_map("S(ssi)", &customer);  /* pass structure address */
	tpl_pack(tn, 0);
	tpl_dump(tn, TPL_FILE, "struct.tpl");
	tpl_dump(tn, TPL_MEM, &addr, &len);
	tpl_free(tn);
	tn = 0x00;

	//Store serialized data in to memcache
	memcached_st *memc = memcached(config_string, strlen(config_string));
	memcached_return_t rc = memcached_set(memc, "customer", strlen("customer"), addr, len, (time_t)0, (uint32_t)0);

	if (rc == MEMCACHED_SUCCESS)
	{
		printf("Success\n");
	}
	else
	{
		error("Error setting value on the memcache servers: %s", config_string);
	}

	//Retrieve the datum back from memcache
	memcache_value = memcached_get(memc, "customer", strlen("customer"), &memcache_value_len, &memcache_value_flags, memcache_error);
	if(memcache_value > 0)
	{
		tn = tpl_map("S(ssi)", &expanded);
		tpl_load(tn, TPL_MEM, memcache_value, memcache_value_len);
		tpl_unpack(tn, 0);
		tpl_free(tn);
		fprintf(stdout, logfmt, "DE-SERIALIZATION", expanded.first_name, expanded.last_name, expanded.age);

	}
	//Clean Up
	memcached_free(memc);
	free(addr);


	return (0);
}
