#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Serialization.h"

// Implementation of SerializableMessage functions

void SerializableMessage_to_bin(Serializable* base)
{
    SerializableMessage* serializable = (SerializableMessage*)base;
    // Convert MyMessage to binary representation and store in serializable->_data
    // Set the size of the serialized data in serializable->_size
    // ...
}

int SerializableMessage_from_bin(Serializable* base, char* data)
{
    SerializableMessage* serializable = (SerializableMessage*)base;
    // Reconstruct MyMessage from binary data and store in serializable->message
    // Return 0 on success, -1 on failure
    // ...
}

char* SerializableMessage_data(Serializable* base)
{
    SerializableMessage* serializable = (SerializableMessage*)base;
    // Return a pointer to the serialized data (serializable->_data)
    return serializable->base._data;
}

int32_t SerializableMessage_size(Serializable* base)
{
    SerializableMessage* serializable = (SerializableMessage*)base;
    // Return the size of the serialized data (serializable->_size)
    return serializable->base._size;
}

// Create a new SerializableMessage instance
SerializableMessage* new_SerializableMessage()
{
    SerializableMessage* serializable = malloc(sizeof(SerializableMessage));
    serializable->base._size = 0;
    serializable->base._data = NULL;
    serializable->base.to_bin = (void (*)(Serializable*))SerializableMessage_to_bin;
    serializable->base.from_bin = (int (*)(Serializable*, char*))SerializableMessage_from_bin;
    serializable->base.data = (char* (*)(Serializable*))SerializableMessage_data;
    serializable->base.size = (int32_t (*)(Serializable*))SerializableMessage_size;
    // Initialize other members of SerializableMessage if needed
    // ...
    return serializable;
}

// Free the memory allocated for a SerializableMessage instance
void free_SerializableMessage(SerializableMessage* serializable)
{
    free(serializable->base._data);
    free(serializable);
}

// int main()
// {
//     // Example usage
//     SerializableMessage* message = new_SerializableMessage();

//     // Serialize the message
//     message->base.to_bin((Serializable*)message);

//     // Get the serialized data
//     char* serializedData = message->base.data((Serializable*)message);

//     // Get the size of the serialized data
//     int32_t serializedSize = message->base.size((Serializable*)message);

//     // Print the serialized data and size
//     printf("Serialized Data: %s\n", serializedData);
//     printf("Serialized Size: %d\n", serializedSize);

//     // Deserialize the message
//     message->base.from_bin((Serializable*)message, serializedData);

//     // Access the deserialized message fields
//     printf("Deserialized Message ID: %d\n", message->message.messageId);

//     // Free the memory
//     free_SerializableMessage(message);

//     return 0;
// }
