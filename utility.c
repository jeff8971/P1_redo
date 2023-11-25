#include "utility.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>

/**
 * Gets the current time in milliseconds.
 * 
 * @return The current time in milliseconds since the Unix epoch.
 */
long long current_timestamp_ms() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000LL + spec.tv_nsec / 1000000; // Convert to milliseconds
}

/**
 * Generates a random numeric string.
 * 
 * @return A dynamically allocated random numeric string. The caller is responsible for freeing this memory.
 */
char* generate_random_number_string() {
    static int initialized = 0;
    if (!initialized) {
        srand((unsigned int)time(NULL)); // Seed the random number generator once
        initialized = 1;
    }

    int length = rand() % 10 + 1; // Generate a length between 1 and 10
    char* random_number_string = (char*)malloc((length + 1) * sizeof(char)); // Allocate memory for the string

    if (random_number_string) {
        for (int i = 0; i < length; ++i) {
            random_number_string[i] = '0' + (rand() % 10); // Generate a random digit
        }
        random_number_string[length] = '\0'; // Null-terminate the string
    } else {
        fprintf(stderr, "Error: Memory allocation failed for random_number_string.\n");
    }

    return random_number_string;
}
