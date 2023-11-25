#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/time.h>
#include <stdbool.h>

#define MESSAGE_SIZE 1024
#define CONTEXT_SIZE 1024


/**
 * @brief message structure
 */
typedef struct t_message{
    int identifier; // id, unique id of message
    time_t time_sent; // time of the sent time of the message
    char sender[100]; // message sender
    char receiver[100]; // message receiver
    char content[CONTEXT_SIZE]; // message content, max 800 bytes
    int delivered;
} t_message;


// function prototypes
t_message* create_msg(int identifier, const char* sender, const char* receiver, const char* content, int delivered_flag, int limit_size);

#endif // MESSAGE_H
