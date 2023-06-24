//Enrique JG
//Manuel PM

//MESSAGE STRUCTURES AND SERIALIZATION

#ifndef SERIALIZABLE_MESSAGE_H
#define SERIALIZABLE_MESSAGE_H

#include <stdint.h>
#include <stdbool.h>


#define MAX_PLAYERS 12

typedef struct {
    int playerIndex;
    float x, y;
    float radius;
    bool alive;
} Player;

// Define the server's message structure
typedef struct
{
    int messageId;
    Player players[MAX_PLAYERS];

} ServerMessage;


// Define the client's message structure
typedef struct
{
    Player player;

    int timestamp;

} ClientMessage;

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

// Define the SerializableMessage struct implementing Serializable for the server's message
typedef struct
{
    Serializable base;
    ServerMessage message;
} SerializableServerMessage;

// Define the SerializableMessage struct implementing Serializable for the client's message
typedef struct
{
    Serializable base;
    ClientMessage message;
} SerializableClientMessage;

// Implementation of SerializableMessage functions for the server's message

void SerializableServerMessage_to_bin(Serializable* base);
int SerializableServerMessage_from_bin(Serializable* base, char* data);
char* SerializableServerMessage_data(Serializable* base);
int32_t SerializableServerMessage_size(Serializable* base);

// Implementation of SerializableMessage functions for the client's message

void SerializableClientMessage_to_bin(Serializable* base);
int SerializableClientMessage_from_bin(Serializable* base, char* data);
char* SerializableClientMessage_data(Serializable* base);
int32_t SerializableClientMessage_size(Serializable* base);

// Create a new SerializableServerMessage instance
SerializableServerMessage* new_SerializableServerMessage();

// Create a new SerializableClientMessage instance
SerializableClientMessage* new_SerializableClientMessage();

// Free the memory allocated for a SerializableServerMessage instance
void free_SerializableServerMessage(SerializableServerMessage* serializable);

// Free the memory allocated for a SerializableClientMessage instance
void free_SerializableClientMessage(SerializableClientMessage* serializable);

#endif /* SERIALIZABLE_MESSAGE_H */
