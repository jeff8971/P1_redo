# Cache Mechanism Project

## Overview

This project implements a caching system with support for Least Recently Used (LRU) and Random Replacement strategies. It simulates storing and retrieving messages in a cache, allowing users to analyze the efficiency of different cache replacement strategies.

## Design

- **Message Structure**: Defines the structure for messages including identifiers, timestamps, sender, receiver, content, etc.
- **Cache Implementation**: Uses a hash table with linked lists for collision handling. Each cache entry is associated with an LRU node to track access patterns.
- **LRU and Random Replacement**: The LRU strategy moves frequently accessed items to the front, while the Random Replacement strategy evicts cache entries randomly when necessary.
- **Cache Operations**: Supports storing and retrieving messages from the cache, with the ability to search on disk if the message is not found in the cache.

## How to Compile and Run

### Compiling the Program

1. **Using the Makefile**:
   - Ensure you have the `Makefile` in the project directory.
   - Run the following command to compile:
     ```bash
     make
     ```
   - This command will create two executables: `program` for the main program and `test_program` for testing.

### Running the Main Program

1. **Execute the Main Program**:
   - Run the compiled program with the desired replacement strategy:
     ```bash
     ./program 0  # For LRU strategy
     ./program 1  # For Random strategy
     ```
   - The program accepts a command-line argument (`0` for LRU, `1` for Random) to select the replacement strategy.

### Running the Test Program

1. **Execute the Test Program**:
   - Run the compiled test program:
     ```bash
     ./test_program
     ```
   - This executes the tests designed to validate the cache mechanism.

## Testing the Cache Mechanism

- **Automated Tests**: The `test_program` executable contains tests to verify the cache's functionality.
- **Test Scenarios**:
  - Storing and retrieving more messages than the cache can hold, triggering the replacement mechanism.
  - Accessing specific messages to alter their LRU order.
  - Verifying that the least recently used messages are replaced in the LRU strategy.
- **Edge Cases and Error Handling**:
  - The tests include handling edge cases like empty cache or cache not yet requiring replacement.
  - Checks for proper handling of `NULL` messages and failed memory allocations.

## Checking the Reports

- After running the main program, check the generated report files for detailed information on the cache's performance.
- **Report Files**:
  - `100_report.txt`: Contains the report for initial 100 message activities.
  - `1000_report.txt`: Includes the simulation report for 1000 message accesses.
  - `100_msg.txt` and `1000_msg.txt`: Provide details of message storage and retrieval activities.
- These files offer insights into cache hits, misses, and overall effectiveness of the chosen cache replacement strategy.

## Additional Notes

- Ensure all dependent files (`message.h`, `cache.h`, `utility.h`, etc.) are included in your project directory.
- Modify and extend the program as necessary to fit specific requirements or implement additional features.
