#ifndef MEMCACHE_API_H
#define MEMCACHE_API_H 1
/*
********************************************************************************
;+
Module:		memcache_api

Purpose:	Provides macro to serialize, store, fetch and deserialize objects
			from memcache.

Notes:		Macros do not support linked lists yet.

Author:		Brian McManus <brian@kickbackpoints.com>,<bmcmanus@gmail.com>
;-
********************************************************************************
*/
#include <stdio.h>
#include <tpl.h>
#include <libmemcached/memcached.h>


#define countof(a) (sizeof(a) / sizeof(a[0]))

#define MEMCACHE_INIT(...) char *memcache_value = 0x00; \
	size_t memcache_value_len = 0; \
	uint32_t memcache_value_flags = 0; \
	memcached_return *memcache_error = 0x00; \
	size_t memcache_error_len = 0; \
	tpl_node *tpl_node = 0x00; \
	void *tpl_addr = 0x00; \
	size_t tpl_addr_len = 0; \
	char _servers[15][50] = { __VA_ARGS__ }; \
	int _servers_iterator = 0; \
	char config_string[2048] = "\000"; \
	char _key_string[2048] = "\000"; \
	for(_servers_iterator = 0; _servers_iterator < countof(_servers); ++_servers_iterator) \
		if(_servers[_servers_iterator][0] != (char) 0x00) \
		{ \
			if(_servers_iterator > 0) \
				strcat(config_string, " "); \
			strcat(config_string, "--SERVER="); \
			strcat(config_string, _servers[_servers_iterator]);	\
		} \
\
	memcached_st *memc = memcached(config_string, strlen(config_string));

#define MEMCACHE_STORE(_key_a, _key_b,  _serialize_map, _data_address, ...) tpl_node = tpl_map(_serialize_map, _data_address); \
	tpl_pack(tpl_node, 0); \
	tpl_dump(tpl_node, TPL_MEM, &tpl_addr, &tpl_addr_len); \
	tpl_free(tpl_node); \
	tpl_node = 0x00; \
	sprintf(_key_string, "%s_%s", _key_a, _key_b); \
	memcached_return_t _servers_return = memcached_set(memc, _key_string, strlen(_key_string), tpl_addr, tpl_addr_len, (time_t)0, (uint32_t)0);

#define MEMCACHE_STORE_LL (_key_a, _key_b,  _serialize_map, _data_address, _data_name_type, _) tpl_node = tpl_map(_serialize_map, _data_address); \


#define MEMCACHE_GET(_key_a, _key_b, _serialize_map, _data_address) 	sprintf(_key_string, "%s_%s", _key_a, _key_b); \
		memcache_value = memcached_get(memc, _key_string, strlen(_key_string), &memcache_value_len, &memcache_value_flags, memcache_error); \
		if(memcache_value > 0) \
		{ \
			tpl_node = tpl_map(_serialize_map, _data_address); \
			tpl_load(tpl_node, TPL_MEM, memcache_value, memcache_value_len); \
			tpl_unpack(tpl_node, 0); \
			tpl_free(tpl_node); \
			tpl_node = 0x00; \
		}

#define MEMCACHE_DESTROY(...)	memcache_value_len = 0; \
	memcache_value_flags = 0; \
	tpl_addr_len = 0; \
	_servers_iterator = 0; \
	if(memc) \
	{ \
		memcached_free(memc); \
		memc = 0x00; \
	} \
	if(memcache_value) \
	{ \
		free(memcache_value); \
		memcache_value = 0x00; \
	} \
	if(memcache_error) \
	{ \
		free(memcache_error); \
		memcache_error = 0x00; \
	} \
	if(tpl_node) \
	{ \
		free(tpl_node); \
		tpl_node = 0x00; \
	} \
	if(tpl_addr) \
	{ \
		free(tpl_addr); \
		tpl_addr = 0x00; \
	}
#endif

