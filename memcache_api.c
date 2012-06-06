/*
********************************************************************************
*
* Module: memcache_api.c
*
* Purpose: Unit Tests for Memcache and TPL serialization library.
*
* APIs:
*  MEMCACHE_INIT("SERVER_IP")
*   - Initialize/Connect up to Fifteen Servers
*
*  MEMCACHE_STORE(left, right,  map, src_address, timeout, ...)
*   - Store data to left:right key
*   - With a specific map like "S(is)" http://tpl.sourceforge.net/userguide.html#_expressing_type
*   - Data address is the pointer to the data type we are storing.
*   - Timeout in seconds from the object should expire on.  Must be less
*     - then 30 days and if more than 30 days a unix timestamp.
*   - Variadic paramaters are the sizes in a map that use a #
*      - aka a _serialize_map of "c#" for a string[50] declatration would require
*	   - ..., timeout, 50);
*
*	MEMCACHE_GET(left, right, map, destination_address, ...)
*
*	MEMCACHE_STORE_LIST(left, right, map, src_address, timeout, STRUCTTYP, flink or next label, ...)
*
*	MEMCACHE_GET_LIST(left, right, map, destination_address, STRUCTTYP, flink or next label, ...)
*
*	MEMCACHE_DESTROY()
*	 - Clean-up objects/structures related to TPL and Memcache
*
* Required Libraries:
* 	lrt 			(time, only unit tests require this)
* 	libmemcached	(memcached c interface)
* 	tpl				(serialization library)
*
* Notes:
*  Now supports linked lists.
*  May not support other array types at this time.
*  Performed quite a few valgrind tests on it.  Appears to free all of the
*  memory the wrapper allocates.
*
* Unit Tests:
* gcc -gdwarf-2 -g3 -c -g -I. -D_MAIN_ -I../include memcache_api.c
* gcc -D_MAIN_ -gdwarf-2 -g3 -Wl,-rpath,/usr/lib -g -I. -lrt -ltpl -lmemcached -o memcache_api memcache_api.o -L/usr/lib
* ./memcache_api
* // -> to test linking problems gcc -v -Wl,-rpath,/usr/lib -Wl,-m,elf_x86_64 -Wl,-m,elf_x86_64 -g -I. -ltpl -lmemcached -o memcache_api memcache_api.c -L/usr/lib
********************************************************************************
*/


/* Includes *******************************************************************/
#include <libmemcached-1.0/memcached.h>
#include <tpl.h>
#include "memcache_api.h"


#ifdef _MAIN_
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

typedef struct person
{
    char *first_name;
    char *last_name;
    int age;
    double dollars;
    char label[50];
    int years_service;
} PERSON;
#define _PERSON_MAP "S(ssifc#i)"

typedef struct list
{
	int RecordID;
	double balance;
	struct list *flink;
} LIST;

#define _LIST_MAP "A(S(if))"


void accumulatetime
(
	double start,
	double stop,
	double * storetime,
	char *label
)
{
	double accum = 0.0;
	accum = stop - start;
	fprintf(stdout, "> %s Time Elapsed: %0f\n", label, accum);
	if (storetime)
	{
		*storetime += accum;
		fprintf(stdout, "> Total Time so far: %0f\n\n", *storetime);
	}
}

double marktime()
{
	struct timespec time = { 0, 0 };

#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	time.tv_sec = mts.tv_sec;
	time.tv_nsec = mts.tv_nsec;
#else

	clock_gettime(CLOCK_REALTIME, &time);
#endif
	return((double) time.tv_sec + (double) time.tv_nsec / (double) 1000000000L);
}

int main(int argc, char *argv[])
{
	double start = 0.00;
	double stop = 0.00;
	double total = 0.00;

	const char *personLogFMT = "\n%s\n----------------------------\nfirst_name\t= %s\nlast_name\t= %s\nage\t\t= %d\ndollars\t\t= %f\nYears\t\t= %d\nLabel\t\t= %s\n\n";

	/* Test A single data type. */
	PERSON customer = {"Brian\000", "McManus\000", 72, 32.1F, {0x00}, 12};
	strcpy(customer.label, "Engineer");
	PERSON customer_expanded = {"\000", "\000", 0, 0.0F, {0x00} , 0};

	//Init
	start = marktime();
	MEMCACHE_INIT("192.168.10.170", "192.168.10.169");
	stop = marktime();
	accumulatetime(start, stop, &total, "Initialization/Connection");

	//Log
	fprintf(stdout, personLogFMT, "BEFORE SERIALIZATION", customer.first_name, customer.last_name, customer.age, customer.dollars, customer.years_service, customer.label);

	//Serialize/Store
	start = marktime();
	MEMCACHE_STORE("customer", "12345667", _PERSON_MAP, &customer, 60*60*12, 50);
	stop = marktime();
	accumulatetime(start, stop, &total, "STORAGE OF _PERSON DATATYPE");

	//Retrieve
	start = marktime();
	MEMCACHE_GET("customer", "12345667", _PERSON_MAP, &customer_expanded, 50);
	stop = marktime();
	accumulatetime(start, stop, &total,"RETRIEVAL OF _PERSON DATATYPE");
	fprintf(stdout, personLogFMT, "DE-SERIALIZATION", customer_expanded.first_name, customer_expanded.last_name, customer_expanded.age, customer_expanded.dollars, customer_expanded.years_service, customer_expanded.label);



	/* Test a linked list. */
	LIST *balances = 0x00, *balances_iterator = 0x00, *balances_expanded = 0x00;
	balances = calloc(1, sizeof(LIST));
	balances->RecordID = 1;
	balances->balance = 20.00F;
	balances->flink = calloc(1, sizeof(LIST));

	balances_iterator = balances->flink;
	balances_iterator->RecordID = 2;
	balances_iterator->balance = 9.10F;

	//Serialize/Store
	start = marktime();
	MEMCACHE_STORE_LIST("balance", "12345667", _LIST_MAP, balances, 60*60*12, LIST, flink, NULL);
	stop = marktime();
	accumulatetime(start, stop, &total, "STORAGE OF _LIST DATATYPE");

	//Retrieve
	balances_expanded = calloc(1, sizeof(LIST));
	start = marktime();
	MEMCACHE_GET_LIST("balance", "12345667", _LIST_MAP, balances_expanded, LIST, flink, NULL);
	stop = marktime();
	accumulatetime(start, stop, &total, "RETRIEVAL OF _LIST DATATYPE");

	//Clean-up Variables, Free Memory.
	MEMCACHE_DESTROY();
}
#endif
