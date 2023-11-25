#ifndef CACHE_H
#define CACHE_H

#include "message.h"
#include <sys/time.h>

#define CACHE_SIZE 16
/**
 * @brief message status structure, used to return the status of the message
 */
typedef struct t_message_status{
    t_message message;
    int hit_status; // 1: in cache, 2: on disk, 3: not found
} t_message_status;

/**
 * @brief lru node structure, double linked list
 * 
 */
typedef struct t_lru_node {
    int key;
    struct t_lru_node *prev;
    struct t_lru_node *next;
} t_lru_node;

/**
 * @brief the hash table entry structure, used to store the message in the hash table, and the lru node
 */
typedef struct t_cache_hash_entry{
    int key; // id % hashsize
    struct t_message_status message_with_status; 
    struct t_lru_node *lru_node;
    time_t time_search;
    struct t_cache_hash_entry *next;
} t_cache_hash_entry;


typedef struct t_lru_cache{
    struct t_lru_node *head;
    struct t_lru_node *tail;
} t_lru_cache;


void add_node_to_lru_head(t_lru_cache *lru_cache, t_lru_node *node);
void remove_node_from_lru(t_lru_cache *lru_cache, t_lru_node *node);
void move_node_to_lru_head(t_lru_cache *lru_cache, t_lru_node *node);

int lru_replacement(t_cache_hash_entry *cache_hash_table[], int hash_table_size, t_lru_cache *lru_cache, int *cache_count);
int random_replacement(t_cache_hash_entry *cache_hash_table[], int hash_table_size, t_lru_cache *lru_cache, int *cache_count);

t_message_status* retrieve_msg(int identifier, t_cache_hash_entry* cache_hash_table[], int hash_table_size, t_lru_cache* lru_cache, int* cache_count, int rep_strategy);
void store_msg(const t_message* msg, t_cache_hash_entry* cache_hash_table[], int hash_table_size, t_lru_cache* lru_cache, int* cache_count, int rep_strategy);




#endif // CACHE_H
