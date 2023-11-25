#include "message.h"
#include "cache.h"
#include "utility.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>


// Define the size of the hash table for the cache
#define HASH_TABLE_SIZE 20  // Adjust size as needed

// Define the cache hash table
t_cache_hash_entry* cache_hash_table[HASH_TABLE_SIZE];

// Define the LRU cache
t_lru_cache lru_cache;

// Define the cache count
int cache_count = 0;

// Define replacement strategies
#define LRU 0
#define RANDOM 1


// Define a function pointer type for test functions
typedef void (*TestFunction)(void);

// A simple structure for test cases
typedef struct {
    char* name;
    TestFunction function;
} TestCase;

// Test function prototypes
void test_cache_hit();
void test_cache_miss_disk_search();
void test_cache_miss_not_found();
void test_lru_eviction();
void test_random_eviction();

// Test runner function
void run_test(TestCase test) {
    printf("Running test: %s\n", test.name);
    test.function();
    printf("Test passed: %s\n\n", test.name);
}

// Assertion function
void assert_true(int condition, const char* message) {
    if (!condition) {
        printf("Assertion failed: %s\n", message);
        exit(EXIT_FAILURE);
    }
}


// Main function
int main() {
    TestCase tests[] = {
        {"Cache Hit Test", test_cache_hit},
        {"Cache Miss and Disk Search Test", test_cache_miss_disk_search},
        {"Cache Miss and Not Found Test", test_cache_miss_not_found},
        {"LRU Eviction Test", test_lru_eviction},
        {"Random Eviction Test", test_random_eviction},
        {NULL, NULL}  // Sentinel to mark the end of the array
    };

    for (int i = 0; tests[i].function != NULL; i++) {
        run_test(tests[i]);
    }

    printf("All tests passed successfully.\n");
    return 0;
}



void test_cache_hit() {
    // Initialize cache (if needed)
    memset(cache_hash_table, 0, sizeof(cache_hash_table));
    lru_cache.head = NULL;
    lru_cache.tail = NULL;
    cache_count = 0;

    t_message* msg = create_msg(1, "Alice", "Bob", "Hello, Bob!", 1, MESSAGE_SIZE);
    assert_true(msg != NULL, "Failed to create a message");
    
    // Store the message in the cache (assumes store_msg does not keep the original pointer)
    store_msg(msg, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, LRU);

    // Free msg if it's not kept by store_msg
    free(msg);

    // Retrieve the message from the cache
    t_message_status* retrieved = retrieve_msg(1, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, LRU);
    assert_true(retrieved != NULL, "Failed to retrieve message from cache");
    assert_true(retrieved->hit_status == 1, "Cache hit failed");
    // Clean up
    if (msg) {
        free(msg);

    }
    if (retrieved && retrieved->hit_status != 3) {  // Do not free if not found
        free(retrieved);

    }
}


void test_cache_miss_disk_search() {
    // Initialize cache
    memset(cache_hash_table, 0, sizeof(cache_hash_table));
    lru_cache.head = NULL;
    lru_cache.tail = NULL;
    cache_count = 0;

    int test_id = 100; // Unique identifier for the test message

    // Manually create a file with the test message (simulate disk storage)
    FILE *file = fopen("messages.txt", "a"); // Open file in append mode
    if (file) {
        fprintf(file, "%d %ld Alice Bob 'Message on disk' 1\n", test_id, (long)time(NULL));
        fclose(file);
    } else {
        printf("Failed to open file for writing test message.\n");
        return;
    }

    // Attempt to retrieve the message - it should cause a disk search
    t_message_status* retrieved = retrieve_msg(test_id, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, LRU);
    assert_true(retrieved != NULL, "Failed to retrieve message from disk");
    assert_true(retrieved->hit_status == 2, "Cache miss disk search failed"); // 2 indicates found on disk

    // Clean up
    if (retrieved && retrieved->hit_status != 3) {  // Do not free if not found
        free(retrieved);
    }
}

void test_cache_miss_not_found() {
    // Initialize cache
    memset(cache_hash_table, 0, sizeof(cache_hash_table));
    lru_cache.head = NULL;
    lru_cache.tail = NULL;
    cache_count = 0;

    int test_id = 999; // Use a unique identifier unlikely to be in cache or on disk

    // Attempt to retrieve the message - it should not be found
    t_message_status* retrieved = retrieve_msg(test_id, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, LRU);

    // Check if the message was not found
    assert_true(retrieved != NULL, "Failed to get a response for message retrieval");
    assert_true(retrieved->hit_status == 3, "Message was unexpectedly found"); // 3 indicates not found

    // Clean up
    free(retrieved);
}


void test_lru_eviction() {
    // Initialize cache
    memset(cache_hash_table, 0, sizeof(cache_hash_table));
    lru_cache.head = NULL;
    lru_cache.tail = NULL;
    cache_count = 0;

    // Fill the cache to its maximum capacity
    for (int i = 0; i < CACHE_SIZE; i++) {
        t_message* msg = create_msg(i, "Sender", "Receiver", "Content", 1, MESSAGE_SIZE);
        assert_true(msg != NULL, "Failed to create a message");
        store_msg(msg, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, LRU);
        free(msg);  // Assuming create_msg allocates memory for the message
    }

    // Access one of the messages to change its position in the LRU order
    int accessed_id = 2;  // Example: Accessing the message with ID 2
    retrieve_msg(accessed_id, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, LRU);

    // Add another message to trigger LRU eviction
    t_message* new_msg = create_msg(CACHE_SIZE, "Sender", "Receiver", "New Content", 1, MESSAGE_SIZE);
    assert_true(new_msg != NULL, "Failed to create new message for eviction test");
    store_msg(new_msg, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, LRU);
    free(new_msg);

    // The least recently used message (ID 0) should be evicted, as message ID 2 was accessed
    t_message_status* evicted_status = retrieve_msg(0, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, LRU);
    assert_true(evicted_status != NULL, "Failed to retrieve message after eviction");
    assert_true(evicted_status->hit_status == 2 || evicted_status->hit_status == 3, "LRU eviction failed");

    // Clean up
    if (evicted_status->hit_status != 3) {  // Do not free if not found
        free(evicted_status);
    }
}


void test_random_eviction() {
    // Initialize cache
    memset(cache_hash_table, 0, sizeof(cache_hash_table));
    lru_cache.head = NULL;
    lru_cache.tail = NULL;
    cache_count = 0;

    // Fill the cache to its maximum capacity
    for (int i = 0; i < CACHE_SIZE; i++) {
        t_message* msg = create_msg(i, "Sender", "Receiver", "Content", 1, MESSAGE_SIZE);
        assert_true(msg != NULL, "Failed to create a message");
        store_msg(msg, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, RANDOM);
        free(msg);  // Assuming create_msg allocates memory for the message
    }

    // Add another message to trigger random eviction
    t_message* new_msg = create_msg(CACHE_SIZE, "Sender", "Receiver", "New Content", 1, MESSAGE_SIZE);
    assert_true(new_msg != NULL, "Failed to create new message for eviction test");
    store_msg(new_msg, cache_hash_table, HASH_TABLE_SIZE, &lru_cache, &cache_count, RANDOM);
    free(new_msg);

    // Check if the cache size has remained constant, indicating eviction occurred
    assert_true(cache_count == CACHE_SIZE, "Random eviction did not maintain cache size");

    // Further validation would involve checking the contents of the cache,
    // but this is complicated by the randomness of the eviction policy.
}

