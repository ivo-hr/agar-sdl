#include <string>
#include <vector>
#include <memory>

#include "Message.h"


// Message --------------------------------------------------------------------
Message::Message(MessageType t) : type(t) {
    _data = nullptr;
}

Message::Message(const Message &m){
    alloc_data(MAX_SIZE);
    memcpy(_data, m.data(), MAX_SIZE);
    type = m.type;
}

void Message::to_bin(){
    alloc_data(MAX_SIZE);
    memset(_data, 0, MAX_SIZE);
    memcpy(_data, &type, sizeof(MessageType));
}

int Message::from_bin(char * src){
    alloc_data(MAX_SIZE);
    memcpy(static_cast<void*>(_data), src, MAX_SIZE);
    memcpy(&type, src, sizeof(MessageType));
    return 0;
}

// LoginMessage ----------------------------------------------------------------
LoginMessage::LoginMessage() : Message(LOGIN), name("default"), size(strlen(name.c_str())) {
    MSG_SIZE = strlen(name.c_str()) + sizeof(size_t);
}

LoginMessage::LoginMessage(std::string nam) : Message(LOGIN), name(name), size(strlen(name.c_str())) {
    MSG_SIZE = strlen(name.c_str()) + sizeof(size_t);
};

void LoginMessage::to_bin(){
    alloc_data(MAX_SIZE);
    memset(_data, 0, MSG_SIZE);

    char * tmp = _data;
    memcpy(tmp, &type, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(tmp, &size, sizeof(size_t));
    tmp += sizeof(size_t);

    const char * name_c = name.c_str();
    memcpy(tmp, name_c, size);
}

int LoginMessage::from_bin(char * src){
    alloc_data(MAX_SIZE);
    char * tmp = src;
    memcpy(static_cast<void*>(_data), tmp, MAX_SIZE);
    memcpy(&type, tmp, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(&size, tmp, sizeof(size_t));
    tmp += sizeof(size_t);

    char * name_c = new char[size];
    memcpy(name_c, tmp, size);
    name = std::string(name_c);

    return 0;
}

// PositionMessage -------------------------------------------------------------
