//Enrique JG
//Manuel PM

//MESSAGE STRUCTURES AND SERIALIZATION

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

    int32_t size = sizeof(message->messageId) + sizeof(Player) * MAX_PLAYERS;
    char* data = (char*)malloc(size);

    char* ptr = data;

    memcpy(ptr, &(message->messageId), sizeof(message->messageId));
    ptr += sizeof(message->messageId);

    for (int i = 0; i < MAX_PLAYERS; i++) {
        memcpy(ptr, &(message->players[i]), sizeof(Player));
        ptr += sizeof(Player);
    }

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

    for (int i = 0; i < MAX_PLAYERS; i++) {
        memcpy(&(message->players[i]), ptr, sizeof(Player));
        ptr += sizeof(Player);
    }

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

    int32_t size = sizeof(message->player.playerIndex) + sizeof(message->player.x) + sizeof(message->player.y) + sizeof(message->player.radius) + sizeof(message->player.alive) + sizeof(message->timestamp);

    char* data = (char*)malloc(size);

    char* ptr = data;

    memcpy(ptr, &(message->player.playerIndex), sizeof(message->player.playerIndex));
    ptr += sizeof(message->player.playerIndex);

    memcpy(ptr, &(message->player.x), sizeof(message->player.x));
    ptr += sizeof(message->player.x);

    memcpy(ptr, &(message->player.y), sizeof(message->player.y));
    ptr += sizeof(message->player.y);

    memcpy(ptr, &(message->player.radius), sizeof(message->player.radius));
    ptr += sizeof(message->player.radius);

    memcpy(ptr, &(message->player.alive), sizeof(message->player.alive));
    ptr += sizeof(message->player.alive);

    memcpy(ptr, &(message->timestamp), sizeof(message->timestamp));

    serializable->base._size = size;
    serializable->base._data = data;
}

int SerializableClientMessage_from_bin(Serializable* base, char* data)
{
    SerializableClientMessage* serializable = (SerializableClientMessage*)base;
    ClientMessage* message = &(serializable->message);

    char* ptr = data;

    memcpy(&(message->player.playerIndex), ptr, sizeof(message->player.playerIndex));
    ptr += sizeof(message->player.playerIndex);

    memcpy(&(message->player.x), ptr, sizeof(message->player.x));
    ptr += sizeof(message->player.x);

    memcpy(&(message->player.y), ptr, sizeof(message->player.y));
    ptr += sizeof(message->player.y);

    memcpy(&(message->player.radius), ptr, sizeof(message->player.radius));
    ptr += sizeof(message->player.radius);

    memcpy(&(message->player.alive), ptr, sizeof(message->player.alive));
    ptr += sizeof(message->player.alive);

    memcpy(&(message->timestamp), ptr, sizeof(message->timestamp));

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

void free_SerializableServerMessage(SerializableServerMessage* serializable)
{
    free(serializable->base._data);
    free(serializable);
}

void free_SerializableClientMessage(SerializableClientMessage* serializable)
{
    free(serializable->base._data);
    free(serializable);
}


int main()
{
    // Create a sample server message
    ServerMessage serverMessage;
    serverMessage.messageId = 123;

    Player player1 = {0, 10.0f, 20.0f, 5.0f, true};
    Player player2 = {1, 30.0f, 40.0f, 7.0f, false};
    Player player3 = {2, 50.0f, 60.0f, 9.0f, true};

    serverMessage.players[0] = player1;
    serverMessage.players[1] = player2;
    serverMessage.players[2] = player3;

    // Create a sample client message
    ClientMessage clientMessage;
    clientMessage.player.playerIndex = 2;
    clientMessage.player.x = 15.0f;
    clientMessage.player.y = 25.0f;
    clientMessage.player.radius = 3.0f;

    clientMessage.timestamp = 456;

    // Create SerializableServerMessage instance and serialize the server message
    SerializableServerMessage* serializedServerMessage = new_SerializableServerMessage();
    serializedServerMessage->message = serverMessage;
    serializedServerMessage->base.to_bin((Serializable*)serializedServerMessage);

    // Create SerializableClientMessage instance and serialize the client message
    SerializableClientMessage* serializedClientMessage = new_SerializableClientMessage();
    serializedClientMessage->message = clientMessage;
    serializedClientMessage->base.to_bin((Serializable*)serializedClientMessage);

    // Get the serialized server message data and size
    char* serializedServerData = serializedServerMessage->base.data((Serializable*)serializedServerMessage);
    int32_t serializedServerSize = serializedServerMessage->base.size((Serializable*)serializedServerMessage);

    // Get the serialized client message data and size
    char* serializedClientData = serializedClientMessage->base.data((Serializable*)serializedClientMessage);
    int32_t serializedClientSize = serializedClientMessage->base.size((Serializable*)serializedClientMessage);

    // Print the serialized server message data and size
    printf("Serialized Server Data:\n");
    for (int i = 0; i < serializedServerSize; i++) {
        printf("%02x ", serializedServerData[i] & 0xFF);
    }
    printf("\nSerialized Server Size: %d bytes\n", serializedServerSize);

    // Print the serialized client message data and size
    printf("Serialized Client Data:\n");
    for (int i = 0; i < serializedClientSize; i++) {
        printf("%02x ", serializedClientData[i] & 0xFF);
    }
    printf("\nSerialized Client Size: %d bytes\n", serializedClientSize);

    // Deserialize the server message
SerializableServerMessage* deserializedSerializedServerMessage = new_SerializableServerMessage();
deserializedSerializedServerMessage->base.from_bin((Serializable*)deserializedSerializedServerMessage, serializedServerData);
printf("\nDeserialized Server Message...");

// Deserialize the client message
SerializableClientMessage* deserializedSerializedClientMessage = new_SerializableClientMessage();
deserializedSerializedClientMessage->base.from_bin((Serializable*)deserializedSerializedClientMessage, serializedClientData);
printf("\nDeserialized Client Message...");

// Access the deserialized server message fields
printf("\nDeserialized Server Message ID: %d\n", deserializedSerializedServerMessage->message.messageId);
printf("Deserialized Server Player 1: %d\n", deserializedSerializedServerMessage->message.players[0].playerIndex);
printf("Deserialized Server Player 2: %d\n", deserializedSerializedServerMessage->message.players[1].playerIndex);
printf("Deserialized Server Player 3: %d\n", deserializedSerializedServerMessage->message.players[2].playerIndex);
printf("Deserialized Server Player 4: %d\n", deserializedSerializedServerMessage->message.players[3].playerIndex);
printf("Deserialized Server Player 5: %d\n", deserializedSerializedServerMessage->message.players[4].playerIndex);
printf("Deserialized Server Player 6: %d\n", deserializedSerializedServerMessage->message.players[5].playerIndex);
printf("Deserialized Server Player 7: %d\n", deserializedSerializedServerMessage->message.players[6].playerIndex);
printf("Deserialized Server Player 8: %d\n", deserializedSerializedServerMessage->message.players[7].playerIndex);


// Access the deserialized client message fields
printf("Deserialized Client Player Index: %d\n", deserializedSerializedClientMessage->message.player.playerIndex);
printf("Deserialized Client X: %.2f\n", deserializedSerializedClientMessage->message.player.x);
printf("Deserialized Client Y: %.2f\n", deserializedSerializedClientMessage->message.player.y);
printf("Deserialized Client Radius: %.2f\n", deserializedSerializedClientMessage->message.player.radius);
printf("Deserialized Client Timestamp: %d\n", deserializedSerializedClientMessage->message.timestamp);

// Free the memory
free_SerializableServerMessage(deserializedSerializedServerMessage);
free_SerializableClientMessage(deserializedSerializedClientMessage);


    return 0;
}