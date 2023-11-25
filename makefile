# Makefile for compiling a project with main.c, message.c, cache.c, utility.c, and test.c

# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Name of the executable to create
TARGET = program
TEST_TARGET = test_program

# Source files
SOURCES = main.c message.c cache.c utility.c
TEST_SOURCES = test.c message.c cache.c utility.c

# Object files
OBJECTS = $(SOURCES:.c=.o)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

# Header files
HEADERS = message.h cache.h utility.h

# Default target
all: $(TARGET) $(TEST_TARGET)

# Rule for linking the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Rule for linking the test executable
$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJECTS)

# Rule for compiling source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

# Clean target for removing compiled files
clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECTS)

# Phony targets
.PHONY: all clean
