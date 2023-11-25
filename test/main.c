#include "message.h"
#include "cache.h"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>


#define MESSAGE_NUM 1000;



int main(int argc, char *argv[]) {
    // Open a file for writing the output
    FILE *fp = fopen("report.txt", "w");
    if (fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    // Check command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: program [0 for LRU | 1 for Random]\n");
        return EXIT_FAILURE;
    }

    // Determine the replacement strategy based on the argument
    int replacement_strategy = -1;
    if (strcmp(argv[1], "0") == 0) {
        replacement_strategy = 0;
        printf("------> Activating LRU Cache Replacement Strategy\n");
    } else if (strcmp(argv[1], "1") == 0) {
        replacement_strategy = 1;
        printf("------> Activating Random Cache Replacement Strategy\n");
    } else {
        fprintf(stderr, "Invalid argument. Please use 0 for LRU or 1 for Random.\n");
        return EXIT_FAILURE;
    }

    // Initialize cache-related variables
    int cache_count = 0;
    t_cache_hash_entry* cache_hash_table[CACHE_SIZE];
    t_lru_cache lru_cache = {NULL, NULL};

    // Initialize the cache hash table
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache_hash_table[i] = NULL;
    }

    // Generating and storing messages
    printf("Generating and Storing 20 Messages...\n");
    for (int i = 0; i < 20; i++) {
        usleep(1000);
        char* content = generate_random_number_string(); // Generate a random string of 10 digits
        t_message* msg = create_msg(i, "Sender", "Receiver", content, 0, CONTEXT_SIZE);
        if (msg) {
            printf("Message Created: ID = %d, Timestamp = %ld, Content = %s\n", msg->identifier, msg->time_sent, msg->content);
            store_msg(msg, cache_hash_table, CACHE_SIZE, &lru_cache, &cache_count, replacement_strategy);
            free(msg);
        }
        free(content);
    }

    // Retrieving previously stored messages
    printf("Retrieving Previously Stored Messages...\n");
    for (int i = 0; i < 20; i++) {
        t_message_status* r_msg = retrieve_msg(i, cache_hash_table, CACHE_SIZE, &lru_cache, &cache_count, replacement_strategy);
        if (r_msg && r_msg->hit_status != 3) {
            printf("Message ID %d Retrieved - Sender: %s, Receiver: %s, Content: %s\n", 
                    r_msg->message.identifier, r_msg->message.sender, 
                    r_msg->message.receiver, r_msg->message.content);
        } else {
            printf("Message ID %d Not Found in Cache.\n", i);
        }
    }

    // Cache hit and miss statistics
    int hits = 0, misses = 0;
    int test_set[1000];
    for (int i = 0; i < 1000; i++) {
        test_set[i] = rand() % 20;
    }

    printf("Simulating 1000 Random Cache Accesses...\n");
    for (int i = 0; i < 1000; i++) {
        usleep(1000);
        t_message_status* status = retrieve_msg(test_set[i], cache_hash_table, CACHE_SIZE, &lru_cache, &cache_count, replacement_strategy);
        if (status && status->hit_status == 1) {
            hits++;
        } else {
            misses++;
        }
    }

    printf("%s Strategy - Hits: %d, Misses: %d, Hit Rate: %.2f%%\n", 
           (replacement_strategy == 0) ? "LRU" : "Random", hits, misses, (double)hits / (hits + misses) * 100);

    // Clean up the cache
    for (int i = 0; i < CACHE_SIZE; i++) {
        t_cache_hash_entry *current_entry = cache_hash_table[i];
        while (current_entry) {
            t_cache_hash_entry *next_entry = current_entry->next;
            free(current_entry);
            current_entry = next_entry;
        }
    }

    return 0;
}
