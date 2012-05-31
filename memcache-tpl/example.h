#ifndef EXAMPLE_H
#include <stdio.h>
#include <tpl.h>
#include <libmemcached/memcached.h>
typedef struct person {
    char first_name[50];
    char last_name[50];
    int age;
} PERSON;
#endif
