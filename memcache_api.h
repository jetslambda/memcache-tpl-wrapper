#ifndef MEMCACHE_API_H
#define MEMCACHE_API_H 1
/*
********************************************************************************
;+
* Module:		memcache_api
*
* Purpose:		Provides macro to serialize, store, fetch and deserialize objects
* 			from memcache. Uses TPL and Memcached.
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
* 	libmemcached		(memcached c interface)
* 	tpl			(serialization library)
*
* Notes:
*  - Now supports linked lists.
*  - May not support other array types at this time.
*  - Performed quite a few valgrind tests on it.  Appears to free all of the
*    memory the wrapper allocates.
*
* Author:		Brian McManus <bmcmanus@gmail.com>
;-
********************************************************************************
*/
#include <stdio.h>
#include <tpl.h>
#include <libmemcached-1.0/memcached.h>

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

#define MEMCACHE_STORE(_key_a, _key_b,  _serialize_map, _data_address, _data_timeout, ...) tpl_node = tpl_map(_serialize_map, _data_address, __VA_ARGS__); \
		tpl_pack(tpl_node, 0); \
		tpl_dump(tpl_node, TPL_MEM, &tpl_addr, &tpl_addr_len); \
		tpl_free(tpl_node); \
		tpl_node = 0x00; \
		sprintf(_key_string, "%s:%s", _key_a, _key_b); \
		do { memcached_return_t _servers_return = memcached_set(memc, _key_string, strlen(_key_string), tpl_addr, tpl_addr_len, (time_t)_data_timeout, (uint32_t)0); } while(0); \
		MEMCACHE_FREE();


#define MEMCACHE_GET(_key_a, _key_b, _serialize_map, _data_address, ...) 	sprintf(_key_string, "%s:%s", _key_a, _key_b); \
		memcache_value = memcached_get(memc, _key_string, strlen(_key_string), &memcache_value_len, &memcache_value_flags, memcache_error); \
		if(memcache_value > 0) \
		{ \
			tpl_node = tpl_map(_serialize_map, _data_address, __VA_ARGS__); \
			tpl_load(tpl_node, TPL_MEM, memcache_value, memcache_value_len); \
			tpl_unpack(tpl_node, 0); \
		} \
		MEMCACHE_FREE(); \

#define MEMCACHE_STORE_LIST(_key_a, _key_b, _serialize_map, _data_address, _data_timeout, _data_type, _data_next_label, ...) _data_type *_ll_iterator, *_ll_tmp; \
		_ll_tmp = calloc(1, sizeof(_data_type)); \
		tpl_node = tpl_map(_serialize_map, _ll_tmp, __VA_ARGS__); \
		sprintf(_key_string, "%s:%s", _key_a, _key_b); \
		for(_ll_iterator = _data_address; _ll_iterator != NULL; _ll_iterator=_ll_iterator->_data_next_label) \
		{\
			*_ll_tmp = *_ll_iterator;\
			tpl_pack(tpl_node, 1);\
		}\
		tpl_dump(tpl_node, TPL_MEM, &tpl_addr, &tpl_addr_len); \
		free(_ll_tmp); \
		_ll_tmp = 0x00; \
		memcached_return_t _servers_return = memcached_set(memc, _key_string, strlen(_key_string), tpl_addr, tpl_addr_len, (time_t)_data_timeout, (uint32_t)0); \
		MEMCACHE_FREE(); \


#define MEMCACHE_GET_LIST(_key_a, _key_b, _serialize_map, _data_address, _data_type, _data_next_label, ...) 	sprintf(_key_string, "%s:%s", _key_a, _key_b); \
		memcache_value = memcached_get(memc, _key_string, strlen(_key_string), &memcache_value_len, &memcache_value_flags, memcache_error); \
		if(memcache_value > 0) \
		{ \
			_data_type *_ll_iterator = 0x00, *_ll_gettmp = 0x00, *_ll_new = 0x00; \
			_ll_gettmp = calloc(1,sizeof(_data_type)); \
			tpl_node = tpl_map(_serialize_map, _ll_gettmp, __VA_ARGS__); \
			tpl_load(tpl_node, TPL_MEM, memcache_value, memcache_value_len); \
			_ll_iterator = _data_address; \
			while(tpl_unpack(tpl_node,1) > 0) \
			{ \
				*_ll_iterator = *_ll_gettmp; \
			    _ll_iterator->_data_next_label = calloc(1,sizeof(_data_type)); \
			    _ll_iterator = _ll_iterator->_data_next_label; \
			} \
\
			free(_ll_gettmp); \
			_ll_gettmp = 0x00; \
		} \
		MEMCACHE_FREE(); \

#define MEMCACHE_FREE(...)	memcache_value_len = 0; \
	memcache_value_flags = 0; \
	tpl_addr_len = 0; \
	_servers_iterator = 0; \
	if(memcache_value) \
	{ \
		free(memcache_value); \
		memcache_value = 0x00; \
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

