#include "message.h"
#include "cache.h"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>


int main(int argc, char *argv[]) {

    // Check command-line arguments first
    if (argc != 2) {
        fprintf(stderr, "Usage: program [0 for LRU | 1 for Random]\n");
        return EXIT_FAILURE; // No files to close yet, so just return
    }

    // Open file for initial 20 messages report
    FILE *fp_100_report = fopen("100_report.txt", "w");
    if (fp_100_report == NULL) {
        perror("Error opening 100_report.txt");
        return EXIT_FAILURE;
    }

    // Open file for 1000 messages simulation report
    FILE *fp_1000_report = fopen("1000_report.txt", "w");
    if (fp_1000_report == NULL) {
        perror("Error opening 1000_report.txt");
        fclose(fp_100_report); // Close already opened file before exiting
        return EXIT_FAILURE;
    }

    // Open file for storing 20 messages
    FILE *fp_100_msg = fopen("100_msg.txt", "w");
    if (fp_100_msg == NULL) {
        perror("Error opening 100_msg.txt");
        fclose(fp_100_report);
        fclose(fp_1000_report);
        return EXIT_FAILURE;
    }

    // Open file for storing details of 1000 message accesses
    FILE *fp_1000_msg = fopen("1000_msg.txt", "w");
    if (fp_1000_msg == NULL) {
        perror("Error opening 1000_msg.txt");
        fclose(fp_100_report);
        fclose(fp_1000_report);
        fclose(fp_100_msg);
        return EXIT_FAILURE;
    }

    // Determine the replacement strategy based on the argument
    int replacement_strategy;
    if (strcmp(argv[1], "0") == 0) {
        replacement_strategy = 0; // LRU strategy
        fprintf(fp_100_report, "------> Activating LRU Cache Replacement Strategy\n");
        fprintf(fp_1000_report, "------> Activating LRU Cache Replacement Strategy\n");
    } else if (strcmp(argv[1], "1") == 0) {
        replacement_strategy = 1; // Random strategy
        fprintf(fp_100_report, "------> Activating Random Cache Replacement Strategy\n");
        fprintf(fp_1000_report, "------> Activating Random Cache Replacement Strategy\n");
    } else {
        fprintf(stderr, "Invalid argument. Please use 1 for LRU or 0 for Random.\n");
        fclose(fp_100_report);
        fclose(fp_1000_report);
        fclose(fp_100_msg);
        fclose(fp_1000_msg);
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

    // Generate and store 20 messages
    fprintf(fp_100_msg, "Generating and Storing 100 Messages...\n");
    for (int i = 0; i < 100; i++) {
        usleep(1000);  // Sleep for a short time (optional)
        char* content = generate_random_number_string(); // Generate a random string
        t_message* msg = create_msg(i, "Sender", "Receiver", content, 0, CONTEXT_SIZE); // Create a message
        if (msg) {
            fprintf(fp_100_msg, "Message Created: ID = %d, Timestamp = %ld, Content = %s\n", msg->identifier, msg->time_sent, msg->content);
            store_msg(msg, cache_hash_table, CACHE_SIZE, &lru_cache, &cache_count, replacement_strategy); // Store in cache
            free(msg); // Free the message
        }
        free(content); // Free the content string
    }

    // Variables to track hits and misses
    int hits_100 = 0, misses_100 = 0;

    // Retrieve and calculate hit/miss statistics for 100 random message accesses
    fprintf(fp_100_report, "Retrieving and Calculating Hit/Miss Statistics for 100 Messages...\n");
    for (int i = 0; i < 100; i++) {
        t_message_status* r_msg = retrieve_msg(rand() % 100, cache_hash_table, CACHE_SIZE, &lru_cache, &cache_count, replacement_strategy);
        if (r_msg && r_msg->hit_status == 1) {
            hits_100++;
            fprintf(fp_100_report, "Message ID %d Retrieved - Hit\n", r_msg->message.identifier);
        } else {
            misses_100++;
            fprintf(fp_100_report, "Message ID %d Not Found in Cache - Miss\n", i);
        }
    }

    // Print cache hit/miss statistics for 100 accesses
    fprintf(fp_100_report, "100 Accesses - Hits: %d, Misses: %d, Hit Rate: %.2f%%\n", hits_100, misses_100, (double)hits_100 / (hits_100 + misses_100) * 100);

    /***************************************************************************************************************************************************************/
    // Simulate random cache accesses for statistics
    fprintf(fp_1000_report, "Simulating 1000 Random Cache Accesses...\n");
    int hits = 0, misses = 0;
    for (int i = 0; i < 1000; i++) {
        usleep(1000); // Sleep for a short time (optional)
        int random_id = rand() % 100; // Generate a random message ID
        t_message_status* status = retrieve_msg(random_id, cache_hash_table, CACHE_SIZE, &lru_cache, &cache_count, replacement_strategy);
        if (status && status->hit_status == 1) {
            hits++; // Increment hits
            fprintf(fp_1000_msg, "Message ID %d Retrieved\n", random_id);
        } else {
            misses++; // Increment misses
            fprintf(fp_1000_msg, "Message ID %d Not Found in Cache\n", random_id);
        }
    }

    // Print cache hit/miss statistics
    fprintf(fp_1000_report, "%s Strategy - Hits: %d, Misses: %d, Hit Rate: %.2f%%\n", 
            (replacement_strategy == 0) ? "LRU" : "Random", hits, misses, 
            (double)hits / (hits + misses) * 100);

    // Clean up the cache and free resources
    for (int i = 0; i < CACHE_SIZE; i++) {
        t_cache_hash_entry *current_entry = cache_hash_table[i];
        while (current_entry) {
            t_cache_hash_entry *next_entry = current_entry->next;
            free(current_entry); // Free each cache entry
            current_entry = next_entry;
        }
    }

    // Close files at the end
    fclose(fp_100_report);
    fclose(fp_1000_report);
    fclose(fp_100_msg);
    fclose(fp_1000_msg);

    return 0;
}