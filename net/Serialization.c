#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Serialization.h"

// Implementation of SerializableServerMessage functions

void SerializableServerMessage_to_bin(Serializable* base)
{
    SerializableServerMessage* serializable = (SerializableServerMessage*)base;
    ServerMessage* message = &(serializable->message);

    int32_t size = sizeof(message->messageId) + sizeof(message->players);
    char* data = (char*)malloc(size);

    char* ptr = data;

    memcpy(ptr, &(message->messageId), sizeof(message->messageId));
    ptr += sizeof(message->messageId);

    memcpy(ptr, message->players, sizeof(message->players));

    serializable->base._size = size;
    serializable->base._data = data;
}

int SerializableServerMessage_from_bin(Serializable* base, char* data)
{
    SerializableServerMessage* serializable = (SerializableServerMessage*)base;
    ServerMessage* message = &(serializable->message);

    char* ptr = data;

    memcpy(&(message->messageId), ptr, sizeof(message->messageId));
    ptr += sizeof(message->messageId);

    memcpy(message->players, ptr, sizeof(message->players));

    return 0;
}

char* SerializableServerMessage_data(Serializable* base)
{
    SerializableServerMessage* serializable = (SerializableServerMessage*)base;
    return serializable->base._data;
}

int32_t SerializableServerMessage_size(Serializable* base)
{
    SerializableServerMessage* serializable = (SerializableServerMessage*)base;
    return serializable->base._size;
}

// Implementation of SerializableClientMessage functions

void SerializableClientMessage_to_bin(Serializable* base)
{
    SerializableClientMessage* serializable = (SerializableClientMessage*)base;
    ClientMessage* message = &(serializable->message);

    int32_t size = sizeof(message->username) + sizeof(message->x) + sizeof(message->y) + sizeof(message->radius) + sizeof(message->playerIndex);
    char* data = (char*)malloc(size);

    char* ptr = data;

    memcpy(ptr, message->username, sizeof(message->username));
    ptr += sizeof(message->username);

    memcpy(ptr, &(message->x), sizeof(message->x));
    ptr += sizeof(message->x);

    memcpy(ptr, &(message->y), sizeof(message->y));
    ptr += sizeof(message->y);

    memcpy(ptr, &(message->radius), sizeof(message->radius));
    ptr += sizeof(message->radius);

    memcpy(ptr, &(message->playerIndex), sizeof(message->playerIndex));

    serializable->base._size = size;
    serializable->base._data = data;
}

int SerializableClientMessage_from_bin(Serializable* base, char* data)
{
    SerializableClientMessage* serializable = (SerializableClientMessage*)base;
    ClientMessage* message = &(serializable->message);

    char* ptr = data;

    memcpy(message->username, ptr, sizeof(message->username));
    ptr += sizeof(message->username);

    memcpy(&(message->x), ptr, sizeof(message->x));
    ptr += sizeof(message->x);

    memcpy(&(message->y), ptr, sizeof(message->y));
    ptr += sizeof(message->y);

    memcpy(&(message->radius), ptr, sizeof(message->radius));
    ptr += sizeof(message->radius);

    memcpy(&(message->playerIndex), ptr, sizeof(message->playerIndex));

    return 0;
}

char* SerializableClientMessage_data(Serializable* base)
{
    SerializableClientMessage* serializable = (SerializableClientMessage*)base;
    return serializable->base._data;
}

int32_t SerializableClientMessage_size(Serializable* base)
{
    SerializableClientMessage* serializable = (SerializableClientMessage*)base;
    return serializable->base._size;
}

// Create a new SerializableServerMessage instance
SerializableServerMessage* new_SerializableServerMessage()
{
    SerializableServerMessage* serializable = malloc(sizeof(SerializableServerMessage));
    serializable->base._size = 0;
    serializable->base._data = NULL;
    serializable->base.to_bin = (void (*)(Serializable*))SerializableServerMessage_to_bin;
    serializable->base.from_bin = (int (*)(Serializable*, char*))SerializableServerMessage_from_bin;
    serializable->base.data = (char* (*)(Serializable*))SerializableServerMessage_data;
    serializable->base.size = (int32_t (*)(Serializable*))SerializableServerMessage_size;
    return serializable;
}

// Create a new SerializableClientMessage instance
SerializableClientMessage* new_SerializableClientMessage()
{
    SerializableClientMessage* serializable = malloc(sizeof(SerializableClientMessage));
    serializable->base._size = 0;
    serializable->base._data = NULL;
    serializable->base.to_bin = (void (*)(Serializable*))SerializableClientMessage_to_bin;
    serializable->base.from_bin = (int (*)(Serializable*, char*))SerializableClientMessage_from_bin;
    serializable->base.data = (char* (*)(Serializable*))SerializableClientMessage_data;
    serializable->base.size = (int32_t (*)(Serializable*))SerializableClientMessage_size;
    return serializable;
}

// Free the memory allocated for a SerializableServerMessage instance
void free_SerializableServerMessage(SerializableServerMessage* serializable)
{
    free(serializable->base._data);
    free(serializable);
}

// Free the memory allocated for a SerializableClientMessage instance
void free_SerializableClientMessage(SerializableClientMessage* serializable)
{
    free(serializable->base._data);
    free(serializable);
}

// int main()
// {
//     // Example usage
//     SerializableServerMessage* serverMessage = new_SerializableServerMessage();
//     SerializableClientMessage* clientMessage = new_SerializableClientMessage();

//     // Serialize the server message
//     serverMessage->base.to_bin((Serializable*)serverMessage);

//     // Serialize the client message
//     clientMessage->base.to_bin((Serializable*)clientMessage);

//     // Get the serialized server message data
//     char* serializedServerData = serverMessage->base.data((Serializable*)serverMessage);

//     // Get the serialized client message data
//     char* serializedClientData = clientMessage->base.data((Serializable*)clientMessage);

//     // Get the size of the serialized server message data
//     int32_t serializedServerSize = serverMessage->base.size((Serializable*)serverMessage);

//     // Get the size of the serialized client message data
//     int32_t serializedClientSize = clientMessage->base.size((Serializable*)clientMessage);

//     // Print the serialized server message data and size
//     printf("Serialized Server Data: %s\n", serializedServerData);
//     printf("Serialized Server Size: %d\n", serializedServerSize);

//     // Print the serialized client message data and size
//     printf("Serialized Client Data: %s\n", serializedClientData);
//     printf("Serialized Client Size: %d\n", serializedClientSize);

//     // Deserialize the server message
//     serverMessage->base.from_bin((Serializable*)serverMessage, serializedServerData);

//     // Deserialize the client message
//     clientMessage->base.from_bin((Serializable*)clientMessage, serializedClientData);

//     // Access the deserialized server message fields
//     printf("Deserialized Server Message ID: %d\n", serverMessage->message.messageId);

//     // Access the deserialized client message fields
//     printf("Deserialized Client Username: %s\n", clientMessage->message.username);

//     // Free the memory
//     free_SerializableServerMessage(serverMessage);
//     free_SerializableClientMessage(clientMessage);

//     return 0;
// }
