#include "message.h"
#include "cache.h"
#include "utility.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

extern int cache_count;
extern t_lru_cache lru_cache;

/**
 * Adds a node to the head of the LRU cache.
 * 
 * @param lru_cache Pointer to the LRU cache structure.
 * @param node Pointer to the node to be added.
 */
void add_node_to_lru_head(t_lru_cache *lru_cache, t_lru_node *node) {
    node->next = lru_cache->head;
    node->prev = NULL;

    if (lru_cache->head) {
        lru_cache->head->prev = node;
    }
    lru_cache->head = node;

    if (!lru_cache->tail) {
        lru_cache->tail = node;
    }
}

/**
 * Removes a node from the LRU cache.
 * 
 * @param lru_cache Pointer to the LRU cache structure.
 * @param node Pointer to the node to be removed.
 */
void remove_node_from_lru(t_lru_cache *lru_cache, t_lru_node *node) {
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        lru_cache->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        lru_cache->tail = node->prev;
    }
}

/**
 * Moves a node to the head of the LRU cache.
 * 
 * @param lru_cache Pointer to the LRU cache structure.
 * @param node Pointer to the node to be moved.
 */
void move_node_to_lru_head(t_lru_cache *lru_cache, t_lru_node *node) {
    remove_node_from_lru(lru_cache, node);
    add_node_to_lru_head(lru_cache, node);
}

/**
 * Replaces a cache entry using the LRU strategy.
 * 
 * @param cache_hash_table Array of cache hash table entries.
 * @param hash_table_size Size of the hash table.
 * @param lru_cache Pointer to the LRU cache structure.
 * @param cache_count Pointer to the current count of cache entries.
 * @return The key of the replaced cache entry or -1 if the cache was empty.
 */
int lru_replacement(t_cache_hash_entry *cache_hash_table[], int hash_table_size, t_lru_cache *lru_cache, int *cache_count) {
    if (!lru_cache->tail) {
        return -1;
    }

    t_lru_node *lru_node = lru_cache->tail;
    int replaced_key = lru_node->key;

    // Remove the last node from the LRU cache
    int hash_index = replaced_key % hash_table_size;
    t_cache_hash_entry *current = cache_hash_table[hash_index];
    t_cache_hash_entry *prev = NULL;
    bool found = false;

    while (current) {
        if (current->key == replaced_key) {
            found = true;
            if (prev) {
                prev->next = current->next;
            } else {
                cache_hash_table[hash_index] = current->next;
            }

            remove_node_from_lru(lru_cache, current->lru_node);
            free(current->lru_node);
            free(current);
            (*cache_count)--;
            printf("LRU message ID: %d has been removed from cache.\n", replaced_key);
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!found) {
        // Log error or handle inconsistency
        fprintf(stderr, "Error: Inconsistency found in LRU cache.\n");
    }

    return replaced_key;
}



/**
 * Replaces a cache entry using a random replacement strategy.
 * 
 * @param cache_hash_table Array of cache hash table entries.
 * @param hash_table_size Size of the hash table.
 * @param lru_cache Pointer to the LRU cache structure.
 * @param cache_count Pointer to the current count of cache entries.
 * @return The key of the replaced cache entry or -1 if the cache was empty.
 */
int random_replacement(t_cache_hash_entry *cache_hash_table[], int hash_table_size, t_lru_cache *lru_cache, int *cache_count) {
    // If the cache is empty, return -1
    if (*cache_count == 0) {
        return -1;
    }
    
    // Find a random entry in the hash table
    srand((unsigned int) time(NULL)); // Seed the random number generator (only once per program execution)
    int random_index = rand() % hash_table_size;
    t_cache_hash_entry *current = cache_hash_table[random_index];
    t_cache_hash_entry *prev = NULL;

    // if the selected entry is empty, find the next non-empty entry
    while (!current) {
        random_index = (random_index + 1) % hash_table_size;
        current = cache_hash_table[random_index];
    }

    // randomly select in the entry for replacement
    int entries_count = 0;
    t_cache_hash_entry *temp = current;
    while (temp) {
        entries_count++;
        temp = temp->next;
    }

    int random_entry = rand() % entries_count;
    for (int i = 0; i < random_entry; i++) {
        prev = current;
        current = current->next;
    }

    // replacement action
    int replaced_key = current->key;
    remove_node_from_lru(lru_cache, current->lru_node);

    if (prev) {
        prev->next = current->next;
    } else {
        cache_hash_table[random_index] = current->next;
    }

    free(current->lru_node);
    free(current);
    (*cache_count)--;

    printf("Random replaced message ID: %d has been removed from cache.\n", replaced_key);

    return replaced_key;
}


/**
 * Retrieve a message from the cache.
 * 
 * @param identifier The unique identifier of the message to retrieve.
 * @param cache_hash_table Array of pointers to cache hash table entries.
 * @param hash_table_size Size of the hash table.
 * @param lru_cache Pointer to the LRU cache structure.
 * @param cache_count Pointer to the current count of cache entries.
 * @param rep_strategy Replacement strategy for the cache (0: LRU, 1: Random).
 * @return Pointer to the message status structure, or NULL if not found.
 */
t_message_status* retrieve_msg(int identifier, t_cache_hash_entry* cache_hash_table[], int hash_table_size, t_lru_cache* lru_cache, int* cache_count, int rep_strategy) {
    int hash_index = identifier % hash_table_size;
    t_cache_hash_entry* entry = cache_hash_table[hash_index];

    // Search the cache for the message
    while (entry) {
        if (entry->key == identifier) {
            entry->time_search = current_timestamp_ms();
            entry->message_with_status.hit_status = 1; // 1 indicates found in cache
            move_node_to_lru_head(lru_cache, entry->lru_node);
            printf("Message %d retrieved from cache.\n", identifier);

            return &entry->message_with_status;
        }
        entry = entry->next;
    }

    // Search the disk for the message
    FILE* file = fopen("messages.txt", "r");
    if (file) {
        t_message msg;
        char line[1024];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (sscanf(line, "%d %ld %99s %99s %899s %d", &msg.identifier, &msg.time_sent, msg.sender, msg.receiver, msg.content, &msg.delivered) == 6) {
                if (msg.identifier == identifier) {
                    t_message* retrieved_msg = create_msg(msg.identifier, msg.sender, msg.receiver, msg.content, msg.delivered, CONTEXT_SIZE);
                    retrieved_msg->time_sent = msg.time_sent;
                    fclose(file);
                    printf("Message not found in cache, message %d was found in the disk.\n", identifier);

                    if (retrieved_msg) {
                        store_msg(retrieved_msg, cache_hash_table, hash_table_size, lru_cache, cache_count, rep_strategy);


                        t_message_status* msg_status = (t_message_status*)malloc(sizeof(t_message_status));
                        if (!msg_status) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            return NULL;
                        }
                        *msg_status = (t_message_status){ .message = msg, .hit_status = 2 }; // 2 indicates found on disk
                        return msg_status;
                        free(retrieved_msg);
                    }
                }
            }
        }
        fclose(file);
    }

    // message not found on disk
    t_message_status* new_msg_status = (t_message_status*)malloc(sizeof(t_message_status));
    if (!new_msg_status) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }
    *new_msg_status = (t_message_status){ .hit_status = 3 }; // 3 indicates not found
    return new_msg_status;
}


/**
 * Store a message in the cache.
 * 
 * @param msg Pointer to the message to be stored.
 * @param cache_hash_table Array of pointers to cache hash table entries.
 * @param hash_table_size Size of the hash table.
 * @param lru_cache Pointer to the LRU cache structure.
 * @param cache_count Pointer to the current count of cache entries.
 * @param rep_strategy Replacement strategy for the cache (0: LRU, 1: Random).
 */
void store_msg(const t_message* msg, t_cache_hash_entry* cache_hash_table[], int hash_table_size, t_lru_cache* lru_cache, int* cache_count, int rep_strategy) {
    // if the msg is NULL, return
    if (!msg) return;
    // hash the msg identifier
    int id = msg->identifier;
    int hash_index = id % hash_table_size;
    t_cache_hash_entry* new_entry = (t_cache_hash_entry*)malloc(sizeof(t_cache_hash_entry));
    if (!new_entry) {
        fprintf(stderr, "Error: Memory allocation failed for t_cache_hash_entry.\n");
        return;
    }

    t_lru_node* new_node = (t_lru_node*)malloc(sizeof(t_lru_node));
    if (!new_node || !new_entry) {
        fprintf(stderr, "Error: Memory allocation failed for t_lru_node.\n");
        free(new_entry);
        return;
    }

    new_entry->key = id;
    new_entry->message_with_status.message = *msg;
    new_entry->message_with_status.hit_status = 3; // 3 indicates newly added
    new_entry->time_search = current_timestamp_ms();
    new_entry->lru_node = new_node;
    new_entry->next = NULL;

    new_node->key = id;
    new_node->prev = NULL;
    new_node->next = NULL;

    if (*cache_count >= CACHE_SIZE) {
        if (rep_strategy == 1) {
            lru_replacement(cache_hash_table, hash_table_size, lru_cache, cache_count);
        } else if (rep_strategy == 0) {
            random_replacement(cache_hash_table, hash_table_size, lru_cache, cache_count);
        }
    }

    new_entry->next = cache_hash_table[hash_index];
    cache_hash_table[hash_index] = new_entry;
    (*cache_count)++;
    add_node_to_lru_head(lru_cache, new_node);
    printf("Message %d stored in cache.\n", id);

    // Store message in file to disk, check if message already exists
    FILE* file = fopen("./messages.txt", "r");
    int exists = 0;
    if (file) {
        char line[1024];
        while (fgets(line, sizeof(line), file) != NULL) {
            int stored_id;
            if (sscanf(line, "%d", &stored_id) == 1 && stored_id == id) {
                exists = 1;
                break;
            }
        }
        fclose(file);
    }

    // if message does not exist, store it in file
    if (!exists) {
        file = fopen("./messages.txt", "a");
        if (file) {
            fprintf(file, "%d %ld %s %s %s %d\n", msg->identifier, msg->time_sent, msg->sender, msg->receiver, msg->content, msg->delivered);
            printf("Message %d stored in file.\n", id);
            fclose(file);
        } else {
            fprintf(stderr, "Error: Unable to open file messages.txt for writing.\n");
        }
    } else {
        printf("Error: Unable to open file messages.txt for writing.\n");
    }
}