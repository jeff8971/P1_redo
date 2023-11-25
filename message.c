#include "message.h"
#include "utility.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>


/**
 * Create a new message object.
 * 
 * @param unique_id The unique identifier for the message.
 * @param sender The sender of the message.
 * @param receiver The receiver of the message.
 * @param content The content of the message.
 * @param del_flag Delivery flag for the message (0: not delivered, 1: delivered).
 * @param max_size The maximum allowable size of the message content.
 * @return Pointer to the newly created message, or NULL on failure.
 */
t_message* create_msg(int unique_id, const char* sender, const char* receiver, const char* content, int del_flag, int max_size) {
    // Check if content length exceeds the limit
    if (strlen(content) > max_size) {
        fprintf(stderr, "Error: Message content exceeds maximum size.\n");
        return NULL;
    }

    // Allocate memory for the Message structure
    t_message* msg = (t_message*)malloc(sizeof(t_message));
    if (!msg) {
        fprintf(stderr, "Error: Memory allocation failed for t_message.\n");
        return NULL;
    }
    
    // Initialize the fields of the message object
    msg->identifier = unique_id;
    msg->time_sent = current_timestamp_ms();
    strncpy(msg->sender, sender, sizeof(msg->sender) - 1);
    msg->sender[sizeof(msg->sender) - 1] = '\0';
    strncpy(msg->receiver, receiver, sizeof(msg->receiver) - 1);
    msg->receiver[sizeof(msg->receiver) - 1] = '\0';
    strncpy(msg->content, content, sizeof(msg->content) - 1);
    msg->content[sizeof(msg->content) - 1] = '\0';
    msg->delivered = del_flag;

    return msg;
}

