#ifndef SERIALIZABLE_MESSAGE_H
#define SERIALIZABLE_MESSAGE_H

#include <stdint.h>

// Define your message structure
typedef struct
{
    // Define your message fields
    int messageId;
    char username[20];
    int inputX;
    int inputY;

    int size;

} MyMessage;

// Define the Serializable struct
typedef struct Serializable Serializable;
struct Serializable
{
    int32_t _size;
    char* _data;

    void (*to_bin)(Serializable*);
    int (*from_bin)(Serializable*, char*);
    char* (*data)(Serializable*);
    int32_t (*size)(Serializable*);
};

// Define the SerializableMessage struct implementing Serializable
typedef struct
{
    Serializable base;
    MyMessage message;
} SerializableMessage;

// Implementation of SerializableMessage functions

void SerializableMessage_to_bin(Serializable* base);
int SerializableMessage_from_bin(Serializable* base, char* data);
char* SerializableMessage_data(Serializable* base);
int32_t SerializableMessage_size(Serializable* base);

// Create a new SerializableMessage instance
SerializableMessage* new_SerializableMessage();

// Free the memory allocated for a SerializableMessage instance
void free_SerializableMessage(SerializableMessage* serializable);

#endif /* SERIALIZABLE_MESSAGE_H */
