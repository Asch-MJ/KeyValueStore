/* ***********************************************************************
> File Name:   kvstore.h
> Author:      Asch
> GitHub:      https://github.com/Asch-MJ
> Blog:        https://www.asch.blog/
 ************************************************************************/

#ifndef KVSTORE_H
#define KVSTORE_H

#include "nty_coroutine.h"
#include "log.h"
#include <arpa/inet.h>

#define ENABLE_ARRAY 1
#define ENABLE_RBTREE 1
#define ENABLE_BTREE 0
#define ENABLE_HASH 1
#define ENABLE_DHASH 1
#define ENABLE_SKIPTABLE 1
#define ENABLE_MEM_POOL 0
#define ENABLE_LOG 0

#define ENABLE_KVENGINE 1
#define MAX_TOKENS 32
#define CLINET_MSG_LENGTH 1024

#endif

/*
3\r\n
3\r\n
SET\r\n
3\r\n
KEY\r\n
5\r\n
value\r\n
*/

// array
// SET key value
// GET key
// COUNT
// DELETE key
// EXIST key

// rbtree
// RSET key value
// RGET key
// RCOUNT
// RDELETE key
// REXIST key

// btree
// BSET key value
// BGET key
// BCOUNT
// BDELETE key
// BEXIST key

// hash
// HSET key value
// HGET key
// HCOUNT
// HDELETE key
// HEXIST key

// dhash
// DSET key value
// DGET key
// DCOUNT
// DDELETE key
// DEXIST key

// skiptable
// ZSET key value
// ZGET key
// ZCOUNT
// ZDELETE key
// ZEXIST key

// KVS_CMD_ERROR (format error)
// KVS_CMD_QUIT

/*
struct cmd_arg {
    const char *cmd;
    int argc;
};

struct cmd_arg commands[] = {
    {"SET", 2},
    {"GET", 1},
};
*/