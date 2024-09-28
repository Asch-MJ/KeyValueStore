/* ***********************************************************************
> File Name:   dhash.c
> Author:      Asch
> GitHub:      https://github.com/Asch-MJ
> Blog:        https://www.asch.blog/
 ************************************************************************/

#ifndef DHASH_C
#define DHASH_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct kv_node
{
    char *key;
    char *value;
    struct kv_node *next;
} kv_node;

typedef struct dhash_table
{
    kv_node **table; // 指向节点指针数组
} dhash_table;

dhash_table dhash;

// 初始化哈希表
int init_dhashtable(dhash_table *dhash)
{
    dhash->table = malloc(TABLE_SIZE * sizeof(kv_node *));
    if (dhash->table == NULL)
    {
        return -1; // 分配内存失败
    }

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        dhash->table[i] = NULL;
    }

    return 0; // 成功
}

// 销毁哈希表
void dest_dhashtable(dhash_table *dhash)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        kv_node *node = dhash->table[i];
        while (node)
        {
            kv_node *temp = node;
            node = node->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(dhash->table);
}

// 哈希函数
unsigned int hash_function(char *key)
{
    unsigned int dhash = 5381;
    int c;

    while ((c = *key++))
    {
        dhash = ((dhash << 5) + dhash) + c; // dhash * 33 + c
    }

    return dhash % TABLE_SIZE;
}

// 添加键值对到哈希表
int put_kv_dhashtable(dhash_table *dhash, char *key, char *value)
{
    unsigned int index = hash_function(key);

    kv_node *new_node = malloc(sizeof(kv_node));
    if (!new_node)
        return -1;

    new_node->key = strdup(key);
    new_node->value = strdup(value);

    if (!new_node->key || !new_node->value)
    {
        free(new_node);
        return -1;
    }

    // 插入到链头，实现简单冲突处理（链地址法）
    new_node->next = dhash->table[index];
    dhash->table[index] = new_node;

    return 0;
}

// 获取哈希表中的值
char *get_kv_dhashtable(dhash_table *dhash, char *key)
{
    unsigned int index = hash_function(key);

    kv_node *node = dhash->table[index];
    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
            return node->value;
        node = node->next;
    }

    return NULL; // 未找到该键
}

// 查询当前有多少个键值对
int count_kv_dhashtable(dhash_table *dhash)
{
    int count = 0;

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        kv_node *node = dhash->table[i];
        while (node != NULL)
        {
            count++;
            node = node->next;
        }
    }

    return count;
}

// 删除特定的键值对
int delete_kv_dhashtable(dhash_table *dhash, char *key)
{
    unsigned int index = hash_function(key);

    kv_node **indirect = &(dhash->table[index]);
    kv_node *current = *indirect;

    while (current != NULL && strcmp(current->key, key) != 0)
    {
        indirect = &(current->next);
        current = current->next;
    }

    if (current == NULL)
        return -1; // 未找到该键

    // 找到并删除节点
    (*indirect) = current->next;
    free(current->key);
    free(current->value);
    free(current);

    return 0;
}

// 检查特定的键是否存在于哈希表中
int exist_kv_dhashtable(dhash_table *dhash, char *key)
{
    unsigned int index = hash_function(key);

    kv_node *node = dhash->table[index];

    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
            return 1; // 存在该键
        node = node->next;
    }

    return 0; // 不存在该键
}

#if 0
int main()
{
    dhash_table my_hash;

    // 初始化哈希表
    if (init_dhashtable(&my_hash) != 0)
    {
        printf("Failed to initialize the hashtable.\n");
        return -1;
    }

    // 添加一些数据
    put_kv_dhashtable(&my_hash, "name", "Alice");
    put_kv_dhashtable(&my_hash, "age", "30");

    // 获取数据并打印输出
    printf("Name: %s\n", get_kv_dhashtable(&my_hash, "name")); // 输出: Alice

    // 删除数据并验证是否成功删除
    delete_kv_dhashtable(&my_hash, "age");
    printf("Age exists? %d\n", exist_kv_dhashtable(&my_hash, "age")); // 输出: 0

    // 打印当前数量
    printf("Current KV Count: %d\n", count_kv_dhashtable(&my_hash));

    // 销毁哈希表以释放内存
    dest_dhashtable(&my_hash);

    return 0;
}
#endif
#endif