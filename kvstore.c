/* ***********************************************************************
> File Name:   kvstore.c
> Author:      Asch
> GitHub:      https://github.com/Asch-MJ
> Blog:        https://www.asch.blog/
 ************************************************************************/

/*
 *  NtyCo-Author : WangBoJing
 *
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Author. (C) 2017
 */

#ifndef KVSTORE_C
#define KVSTORE_C

#include "kvstore.h"

typedef enum kvs_cmd_e
{
	// array
	KVS_CMD_START = 0,
	KVS_CMD_SET = KVS_CMD_START,
	KVS_CMD_GET,
	KVS_CMD_COUNT,
	KVS_CMD_DELETE,
	KVS_CMD_EXIST,
	// rbtree
	KVS_CMD_RSET,
	KVS_CMD_RGET,
	KVS_CMD_RCOUNT,
	KVS_CMD_RDELETE,
	KVS_CMD_REXIST,
	// btree
	KVS_CMD_BSET,
	KVS_CMD_BGET,
	KVS_CMD_BCOUNT,
	KVS_CMD_BDELETE,
	KVS_CMD_BEXIST,
	// hash
	KVS_CMD_HSET,
	KVS_CMD_HGET,
	KVS_CMD_HCOUNT,
	KVS_CMD_HDELETE,
	KVS_CMD_HEXIST,
	// dhash
	KVS_CMD_DSET,
	KVS_CMD_DGET,
	KVS_CMD_DCOUNT,
	KVS_CMD_DDELETE,
	KVS_CMD_DEXIST,
	// skiptable
	KVS_CMD_ZSET,
	KVS_CMD_ZGET,
	KVS_CMD_ZCOUNT,
	KVS_CMD_ZDELETE,
	KVS_CMD_ZEXIST,

	// cmd
	KVS_CMD_ERROR,
	KVS_CMD_QUIT,
	KVS_CMD_END,
} kvs_cmd_t;

const char *commands[] = {
	"SET",
	"GET",
	"COUNT",
	"DELETE",
	"EXIST",
	"RSET",
	"RGET",
	"RCOUNT",
	"RDELETE",
	"REXIST",
	"BSET",
	"BGET",
	"BCOUNT",
	"BDELETE",
	"BEXIST",
	"HSET",
	"HGET",
	"HCOUNT",
	"HDELETE",
	"HEXIST",
	"DSET",
	"DGET",
	"DCOUNT",
	"DDELETE",
	"DEXIST",
	"ZSET",
	"ZGET",
	"ZCOUNT",
	"ZDELETE",
	"ZEXIST",
};

typedef enum kvs_result_e
{
	KVS_RESULT_SUCCESS,
	KVS_RESULT_FAILED
} kvs_result_t;

const char *result[] = {
	"SUCCESS",
	"FAILED"};

#if 1

void *kvs_malloc(size_t size)
{
	return malloc(size);
}

void kvs_free(void *ptr)
{
	return free(ptr);
}

#endif

#if ENABLE_MEM_POOL

typedef struct mempool_s
{
	int block_size; //
	int free_count;

	char *free_ptr;
	char *mem;
} mempool_t;

mempool_t mem;

#endif

#if ENABLE_ARRAY

typedef struct kvs_array_item_s
{
#if ENABLE_MEM_POOL
	void *next; // 占空项指向next
#endif
	char *key;
	char *value;

} kvs_array_item_t;

#define KVS_ARRAY_ITEM_SIZE 1024

#if ENABLE_MEM_POOL

kvs_array_item_t *array_table;

#else

kvs_array_item_t array_table[KVS_ARRAY_ITEM_SIZE] = {0};

#endif
int array_count = 0;

kvs_array_item_t *kvs_array_search_item(char *key)
{

	if (!key)
		return NULL;
#if ENABLE_MEM_POOL
#define MEM_PAGE_SIZE (KVS_ARRAY_ITEM_SIZE * sizeof(kvs_array_item_t))
	// mem.mem

	mem.block_size = KVS_ARRAY_ITEM_SIZE;

	char *ptr = mem.free_ptr;
	for (size_t i = 0; i < MEM_PAGE_SIZE / sizeof(kvs_array_item_t); i++)
	{
		kvs_array_item_t *item = (kvs_array_item_t *)(ptr + i * sizeof(kvs_array_item_t));

		if (item != NULL && item->key != NULL && 0 == strcmp(item->key, key)) // strcmp()前判断key?=NULL
			return item;
		*(char **)ptr = ptr + sizeof(kvs_array_item_t);
		ptr += sizeof(kvs_array_item_t);
	}
#else

	int i = 0;
	for (i = 0; i < KVS_ARRAY_ITEM_SIZE; i++)
	{
		if (array_table[i].key != NULL && 0 == strcmp(array_table[i].key, key)) // strcmp()前判断key?=NULL
			return &array_table[i];
	}
#endif
	return NULL;
}

// KVS_CMD_EXIST
int kvs_array_exist(char *key)
{
	return (kvs_array_search_item(key) != NULL);
}

// KVS_CMD_SET
int kvs_array_set(char *key, char *value)
{

	if (key == NULL || value == NULL || array_count == KVS_ARRAY_ITEM_SIZE - 1)
		return -1;

	if (kvs_array_exist(key))
		return -1;

	char *kcopy = kvs_malloc(strlen(key) + 1);
	if (kcopy == NULL)
		return -1;
	strncpy(kcopy, key, strlen(key) + 1);

	char *vcopy = kvs_malloc(strlen(value) + 1);
	if (vcopy == NULL)
	{
		free(kcopy);
		return -1;
	}
	strncpy(vcopy, value, strlen(value) + 1);
#if ENABLE_MEM_POOL
	void *memp_alloc(mempool_t * m);
	kvs_array_item_t *item = memp_alloc(&mem);
	item->key = kcopy;
	item->value = vcopy;
	array_count++;

#else

#if 0
	array_table[array_count].key = kcopy;
	array_table[array_count].value = vcopy;
	array_count++;
#else
	int i = 0;
	for (i = 0; i < KVS_ARRAY_ITEM_SIZE; ++i)
	{
		if (array_table[i].key == NULL && array_table[i].value == NULL)
		{
			break;
		}
	}
	array_table[i].key = kcopy;
	array_table[i].value = vcopy;
	array_count++;
#endif
#endif
	return 0;
}

// KVS_CMD_GET
char *kvs_array_get(char *key)
{

	kvs_array_item_t *item = kvs_array_search_item(key);
	if (item)
	{
		return item->value;
	}
	return NULL;
}

// KVS_CMD_COUNT
int kvs_array_count(void)
{
	return array_count;
}

// KVS_CMD_DELETE
int kvs_array_delete(char *key)
{

	if (key == NULL)
		return -1;

	kvs_array_item_t *item = kvs_array_search_item(key);
	if (item == NULL)
	{
		return -1;
	}
	//
	if (item->key)
	{
		kvs_free(item->key);
		item->key = NULL;
	}
	if (item->value)
	{
		kvs_free(item->value);
		item->value = NULL;
	}
	array_count--;
	return 0;
}

#endif

#if ENABLE_MEM_POOL

// #define MEM_PAGE_SIZE (KVS_ARRAY_ITEM_SIZE * sizeof(kvs_array_item_t))

int memp_init(mempool_t *m, int block_size)
{

	if (!m)
		return -2;

	m->block_size = block_size;
	m->free_count = MEM_PAGE_SIZE / block_size;

	m->free_ptr = (char *)malloc(MEM_PAGE_SIZE);
	if (!m->free_ptr)
		return -1;
	m->mem = m->free_ptr;

	int i = 0;
	char *ptr = m->free_ptr;
	for (i = 0; i < m->free_count; i++)
	{

		*(char **)ptr = ptr + block_size;
		ptr += block_size;
	}
	*(char **)ptr = NULL;

	return 0;
}

void *memp_alloc(mempool_t *m)
{

	if (!m || m->free_count == 0)
		return NULL;

	void *ptr = m->free_ptr;

	m->free_ptr = *(char **)ptr;
	m->free_count--;

	return ptr;
}

void *memp_free(mempool_t *m, void *ptr)
{
#if 1
	*(char **)ptr = m->free_ptr;
	m->free_ptr = (char *)ptr;
	m->free_count++;
#else
	if (ptr == NULL)
	{
		return NULL; // 如果传入空指针，则直接返回空指针
	}

	*(char **)ptr = m->free_ptr;
	m->free_ptr = (char *)ptr;
	m->free_count++;

	return ptr; // 返回释放的指针
#endif
	return ptr;
}

void init_array_table(void)
{

	memp_init(&mem, sizeof(kvs_array_item_t));
}

#endif

#if ENABLE_RBTREE

typedef struct _rbtree rbtree_t;
typedef struct _rbtree_node rbtree_node_t;

extern rbtree_t tree;

int init_rbtree(rbtree_t *tree);
void dest_rbtree(rbtree_t *tree);
int put_kv_rbtree(rbtree_t *tree, char *key, char *value);
char *get_kv_rbtree(rbtree_t *tree, char *key);
int count_kv_rbtree(rbtree_t *tree);
int exist_kv_rbtree(rbtree_t *tree, char *key);
int delete_kv_rbtree(rbtree_t *tree, char *key);

extern rbtree_t tree;

int kvs_rbtree_set(char *key, char *value)
{
	return put_kv_rbtree(&tree, key, value);
}
char *kvs_rbtree_get(char *key)
{
	return get_kv_rbtree(&tree, key);
}
int kvs_rbtree_count(void)
{
	return count_kv_rbtree(&tree);
}
int kvs_rbtree_exist(char *key)
{
	return exist_kv_rbtree(&tree, key);
}
int kvs_rbtree_delete(char *key)
{
	return delete_kv_rbtree(&tree, key);
}

#endif

#if ENABLE_BTREE
typedef struct

	init_btree(btree_t *btree);
void dest_btree(btree_t *btree);
int put_kv_btree(btree_t *btree, char *key, char *value);
char *get_kv_btree(btree_t *btree, char *key);
int count_kv_btree(btree_t *btree);
int exist_kv_btree(btree_t *btree, char *key);
int delete_kv_btree(btree_t *btree, char *key);

#endif

#if ENABLE_HASH

typedef struct hashtable_s hashtable_t;

int init_hashtable(hashtable_t *hash);
void dest_hashtable(hashtable_t *hash);
int put_kv_hashtable(hashtable_t *hash, char *key, char *value);
char *get_kv_hashtable(hashtable_t *hash, char *key);
int count_kv_hashtable(hashtable_t *hash);
int delete_kv_hashtable(hashtable_t *hash, char *key);
int exist_kv_hashtable(hashtable_t *hash, char *key);

extern hashtable_t hash;

int kvs_hash_set(char *key, char *value)
{
	return put_kv_hashtable(&hash, key, value);
}
char *kvs_hash_get(char *key)
{
	return get_kv_hashtable(&hash, key);
}
int kvs_hash_count(void)
{
	return count_kv_hashtable(&hash);
}
int kvs_hash_exist(char *key)
{
	return exist_kv_hashtable(&hash, key);
}
int kvs_hash_delete(char *key)
{
	return delete_kv_hashtable(&hash, key);
}
#endif

#if ENABLE_DHASH
typedef struct kv_node dhash_table;

int init_dhashtable(dhash_table *dhash);
void dest_dhashtable(dhash_table *dhash);
int put_kv_dhashtable(dhash_table *dhash, char *key, char *value);
char *get_kv_dhashtable(dhash_table *dhash, char *key);
int count_kv_dhashtable(dhash_table *dhash);
int delete_kv_dhashtable(dhash_table *dhash, char *key);
int exist_kv_dhashtable(dhash_table *dhash, char *key);

extern dhash_table dhash;

int kvs_dhash_set(char *key, char *value)
{
	return put_kv_dhashtable(&dhash, key, value);
}
char *kvs_dhash_get(char *key)
{
	return get_kv_dhashtable(&dhash, key);
}
int kvs_dhash_count(void)
{
	return count_kv_dhashtable(&dhash);
}
int kvs_dhash_exist(char *key)
{
	return exist_kv_dhashtable(&dhash, key);
}
int kvs_dhash_delete(char *key)
{
	return delete_kv_dhashtable(&dhash, key);
}
#endif

#if ENABLE_SKIPTABLE
typedef struct Node SkipList;

int init_skiptable(SkipList *table);
void dest_skiptable(SkipList *table);
int put_kv_skiptable(SkipList *table, char *key, char *value);
char *get_kv_skiptable(SkipList *table, char *key);
int count_kv_skiptable(SkipList *table);
int delete_kv_skiptable(SkipList *table, char *key);
int exist_kv_skiptable(SkipList *table, char *key);

extern SkipList table;

int kvs_skiptable_set(char *key, char *value)
{
	return put_kv_skiptable(&table, key, value);
}
char *kvs_skiptable_get(char *key)
{
	return get_kv_skiptable(&table, key);
}
int kvs_skiptable_count(void)
{
	return count_kv_skiptable(&table);
}
int kvs_skiptable_exist(char *key)
{
	return exist_kv_skiptable(&table, key);
}
int kvs_skiptable_delete(char *key)
{
	return delete_kv_skiptable(&table, key);
}

#endif

#if ENABLE_KVENGINE
void init_kvengine(void)
{
#if ENABLE_RBTREE
	init_rbtree(&tree);
#if ENABLE_LOG
	LOG("init_rbtree SUCCESS");
#endif
#endif
#if ENABLE_HASH
	init_hashtable(&hash);
#if ENABLE_LOG
	LOG("init_hashtable SUCCESS");
#endif
#endif
#if ENABLE_DHASH
	init_dhashtable(&dhash);
#if ENABLE_LOG
	LOG("init_dhash SUCCESS");
#endif
#endif
#if ENABLE_SKIPTABLE
	init_skiptable(&table);
#if ENABLE_LOG
	LOG("init_skiptable SUCCESS");
#endif
#endif
}
void dest_kvengine(void)
{
#if ENABLE_RBTREE
	dest_rbtree(&tree);
#if ENABLE_LOG
	LOG("dest_rbtree SUCCESS");
#endif
#endif
#if ENABLE_HASH
	dest_hashtable(&hash);
#if ENABLE_LOG
	LOG("dest_hashtable SUCCESS");
#endif
#endif
#if ENABLE_DHASH
	dest_dhashtable(&dhash);
#if ENABLE_LOG
	LOG("dest_dhashtable SUCCESS");
#endif
#endif
#if ENABLE_SKIPTABLE
	dest_skiptable(&table);
#if ENABLE_LOG
	LOG("dest_skiptable SUCCESS");
#endif
#endif
}
#endif

// OK
// Success
// SUCCESS

int kvs_parser_protocol(char *msg, char **tokens, int count)
{

	if (tokens == NULL || tokens[0] == NULL || count == 0)
	{
#if ENABLE_LOG
		LOG("kvs_parser_protocol FAILED");
#endif
		return KVS_CMD_ERROR;
	}
	int cmd = KVS_CMD_START;
	for (cmd = KVS_CMD_START; cmd <= KVS_CMD_ZEXIST; cmd++)
	{
		if (0 == strcmp(tokens[0], commands[cmd]))
			break;
	}
	/*
	 * tokens[0]: cmd
	 * tokens[1]: key
	 * tokens[2]: value
	 */

	switch (cmd)
	{
#if ENABLE_ARRAY
	case KVS_CMD_SET:
	{
		assert(count == 3); //
		printf("cmd: %s\n", tokens[0]);
		printf("key: %s\n", tokens[1]);
		printf("value: %s\n", tokens[2]);

		int ret = 0;
		int res = kvs_array_set(tokens[1], tokens[2]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n");
#if ENABLE_LOG
			LOG("SET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("SET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_GET:
	{

		int ret = 0;
		char *value = kvs_array_get(tokens[1]);
		if (value)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "%s\r\n", value);
#if ENABLE_LOG
			LOG("GET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED: NO EXIST\r\n");
#if ENABLE_LOG
			LOG("GET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_COUNT:
	{
		int res = kvs_array_count();

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("COUNT SUCCESS");
#endif

		return ret;
	}
	case KVS_CMD_DELETE:
	{
		int ret = 0;
		int res = kvs_array_delete(tokens[1]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n"); ///
#if ENABLE_LOG
			LOG("DELETE SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("DELETE FAILED");
#endif
		}
		return ret;
	}
	case KVS_CMD_EXIST:
	{

		int res = kvs_array_exist(tokens[1]);

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("EXIT SUCCESS");
#endif

		return ret;
	}
#endif

#if ENABLE_RBTREE
	// rbtree
	case KVS_CMD_RSET:
	{

		int ret = 0;
		int res = kvs_rbtree_set(tokens[1], tokens[2]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n");
#if ENABLE_LOG
			LOG("RSET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("RSET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_RGET:
	{

		int ret = 0;
		char *value = kvs_rbtree_get(tokens[1]);
		if (value)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "%s\r\n", value);
#if ENABLE_LOG
			LOG("RGET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED: NO EXIST\r\n");
#if ENABLE_LOG
			LOG("RGET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_RCOUNT:
	{
		int res = kvs_rbtree_count();

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("RCOUNT SUCCESS");
#endif

		return ret;
	}
	case KVS_CMD_RDELETE:
	{
		int ret = 0;
		int res = kvs_rbtree_delete(tokens[1]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n"); ///
#if ENABLE_LOG
			LOG("RDELETE SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("RDELETE FAILED");
#endif
		}
		return ret;
	}
	case KVS_CMD_REXIST:
	{

		int res = kvs_rbtree_exist(tokens[1]);

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("REXIST SUCCESS");
#endif
		return ret;
	}
#endif
	// btree
	case KVS_CMD_BSET:
		break;
	case KVS_CMD_BGET:
		break;
	case KVS_CMD_BCOUNT:
		break;

	case KVS_CMD_BDELETE:
	case KVS_CMD_BEXIST:
		break;
		// hash
#if ENABLE_HASH
	case KVS_CMD_HSET:
	{

		int ret = 0;
		int res = kvs_hash_set(tokens[1], tokens[2]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n");
#if ENABLE_LOG
			LOG("HSET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("HSET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_HGET:
	{

		int ret = 0;
		char *value = kvs_hash_get(tokens[1]);
		if (value)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "%s\r\n", value);
#if ENABLE_LOG
			LOG("HGET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED: NO EXIST\r\n");
#if ENABLE_LOG
			LOG("HGET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_HCOUNT:
	{
		int res = kvs_hash_count();

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("HCOUNT SUCCESS");
#endif

		return ret;
	}
	case KVS_CMD_HDELETE:
	{
		int ret = 0;
		int res = kvs_hash_delete(tokens[1]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n"); ///
#if ENABLE_LOG
			LOG("HDELETE SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("HDELETE FAILED");
#endif
		}
		return ret;
	}
	case KVS_CMD_HEXIST:
	{

		int res = kvs_hash_exist(tokens[1]);

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("HEXIST SUCCESS");
#endif

		return ret;
	}
#endif
#if ENABLE_DHASH
	case KVS_CMD_DSET:
	{

		int ret = 0;
		int res = kvs_dhash_set(tokens[1], tokens[2]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n");
#if ENABLE_LOG
			LOG("DSET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("DSET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_DGET:
	{

		int ret = 0;
		char *value = kvs_dhash_get(tokens[1]);
		if (value)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "%s\r\n", value);
#if ENABLE_LOG
			LOG("DGET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED: NO EXIST\r\n");
#if ENABLE_LOG
			LOG("DGET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_DCOUNT:
	{
		int res = kvs_dhash_count();

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("DCOUNT SUCCESS");
#endif

		return ret;
	}
	case KVS_CMD_DDELETE:
	{
		int ret = 0;
		int res = kvs_dhash_delete(tokens[1]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n"); ///
#if ENABLE_LOG
			LOG("DDELETE SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("DDELETE FAILED");
#endif
		}
		return ret;
	}
	case KVS_CMD_DEXIST:
	{

		int res = kvs_dhash_exist(tokens[1]);

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("DEXIST SUCCESS");
#endif

		return ret;
	}
#endif
#if ENABLE_SKIPTABLE
	// skiptable
	case KVS_CMD_ZSET:
	{

		int ret = 0;
		int res = kvs_skiptable_set(tokens[1], tokens[2]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n");
#if ENABLE_LOG
			LOG("ZSET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("ZSET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_ZGET:
	{

		int ret = 0;
		char *value = kvs_skiptable_get(tokens[1]);
		if (value)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "%s\r\n", value);
#if ENABLE_LOG
			LOG("ZGET SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED: NO EXIST\r\n");
#if ENABLE_LOG
			LOG("ZGET FAILED");
#endif
		}

		return ret;
	}
	case KVS_CMD_ZCOUNT:
	{
		int res = kvs_skiptable_count();

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("ZCOUNT SUCCESS");
#endif

		return ret;
	}
	case KVS_CMD_ZDELETE:
	{
		int ret = 0;
		int res = kvs_skiptable_delete(tokens[1]);
		if (!res)
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "SUCCESS\r\n"); ///
#if ENABLE_LOG
			LOG("ZDELETE SUCCESS");
#endif
		}
		else
		{
			memset(msg, 0, CLINET_MSG_LENGTH);
			ret = snprintf(msg, CLINET_MSG_LENGTH, "FAILED\r\n");
#if ENABLE_LOG
			LOG("ZDELETE FAILED");
#endif
		}
		return ret;
	}
	case KVS_CMD_ZEXIST:
	{

		int res = kvs_skiptable_exist(tokens[1]);

		memset(msg, 0, CLINET_MSG_LENGTH);
		int ret = snprintf(msg, CLINET_MSG_LENGTH, "%d\r\n", res);
#if ENABLE_LOG
		LOG("ZEXIST SUCCESS");
#endif

		return ret;
	}
#endif
	}
	return 0;
}

// SET Name Asch
int kvs_split_tokens(char **tokens, char *msg)
{

	int count = 0;
	char *token = strtok(msg, " ");

	while (token != NULL)
	{
		tokens[count++] = token;
		token = strtok(NULL, " ");
	}
#if ENABLE_LOG
	LOG("kvs_split_tokens SUCCESS");
#endif

	return count;
}

//
int kvs_protocol(char *msg, int length)
{

	char *tokens[MAX_TOKENS] = {0};
	int count = kvs_split_tokens(tokens, msg);
	int i = 0;

	unused(length);

	for (i = 0; i < count; i++)
	{
		printf("%s\n", tokens[i]);
	}
#if ENABLE_LOG
	LOG("kvs_protocol SUCCESS");
#endif

	return kvs_parser_protocol(msg, tokens, count);
}

///////// network, coroutine

void server_reader(void *arg)
{
	int fd = *(int *)arg;
	int ret = 0;

	while (1)
	{

		char buf[CLINET_MSG_LENGTH] = {0};
		ret = recv(fd, buf, CLINET_MSG_LENGTH, 0);
		if (ret > 0)
		{
			// if(fd > MAX_CLIENT_NUM)
			printf("read from server: %.*s\n", ret, buf);

			ret = kvs_protocol(buf, ret); //

			ret = send(fd, buf, ret, 0);
			if (ret == -1)
			{
				close(fd);
				break;
			}
		}
		else if (ret == 0)
		{
			close(fd);
			break;
		}
	}
#if ENABLE_LOG
	LOG("server_reader SUCCESS");
#endif
}

void server(void *arg)
{

	unsigned short port = *(unsigned short *)arg;
	free(arg);

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		return;

	struct sockaddr_in local, remote;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = INADDR_ANY;
	bind(fd, (struct sockaddr *)&local, sizeof(struct sockaddr_in));

	listen(fd, 20);
	printf("listen port : %d\n", port);

	struct timeval tv_begin;
	gettimeofday(&tv_begin, NULL);

	while (1)
	{
		socklen_t len = sizeof(struct sockaddr_in);
		int cli_fd = accept(fd, (struct sockaddr *)&remote, &len);
		if (cli_fd % 1000 == 999)
		{

			struct timeval tv_cur;
			memcpy(&tv_cur, &tv_begin, sizeof(struct timeval));

			gettimeofday(&tv_begin, NULL);
			int time_used = TIME_SUB_MS(tv_begin, tv_cur);

			printf("client fd : %d, time_used: %d\n", cli_fd, time_used);
		}
		printf("new client comming\n");

		nty_coroutine *read_co;
		nty_coroutine_create(&read_co, server_reader, &cli_fd);
	}
}

void init_kvs_engine(void)
{
	init_rbtree(&tree);
#if ENABLE_LOG
	LOG("init_rbtree success");
#endif
}

int main(int argc, char *argv[])
{
	init_kvengine();
	LOG("init_kvengine started");

	nty_coroutine *co = NULL;

	init_kvs_engine();
	LOG("init_kvs_engine started");

	int i = 0;
	unsigned short base_port = 8000;
	// for (i = 0;i < 100;i ++) {
	unsigned short *port = calloc(1, sizeof(unsigned short));
	unused(argc);
	unused(argv);

	*port = base_port + i;
	nty_coroutine_create(&co, server, port); ////////no run
	//}
	LOG("nty_coroutine_create");

	nty_schedule_run(); // run
	LOG("nty_schedule_run");

	dest_kvengine();
	LOG("dest_kvengine");

	LOG("exit with code 0");
	return 0;
}

#endif