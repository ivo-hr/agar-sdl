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

PositionMessage::PositionMessage() : Message(POSITIONS), size(0) {
    MSG_SIZE = sizeof(size_t);
}

PositionMessage::PositionMessage(std::vector<std::pair<float, float>> pPos) : Message(POSITIONS), size(pPos.size()) {
    MSG_SIZE = sizeof(size_t) + sizeof(float) * 2 * size;
    pos = pPos;
}

void PositionMessage::to_bin(){
    alloc_data(MAX_SIZE);
    memset(_data, 0, MSG_SIZE);

    char * tmp = _data;
    memcpy(tmp, &type, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(tmp, &size, sizeof(size_t));
    tmp += sizeof(size_t);

    for (auto &p : pos) {
        memcpy(tmp, &p.first, sizeof(float));
        tmp += sizeof(float);
        memcpy(tmp, &p.second, sizeof(float));
        tmp += sizeof(float);
    }
}

int PositionMessage::from_bin(char * src){
    alloc_data(MAX_SIZE);
    char * tmp = src;
    memcpy(static_cast<void*>(_data), tmp, MAX_SIZE);
    memcpy(&type, tmp, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(&size, tmp, sizeof(size_t));
    tmp += sizeof(size_t);

    for (size_t i = 0; i < size; ++i) {
        float x, y;
        memcpy(&x, tmp, sizeof(float));
        tmp += sizeof(float);
        memcpy(&y, tmp, sizeof(float));
        tmp += sizeof(float);
        pos.push_back(std::make_pair(x, y));
    }

    return 0;
}

// SizeMessage ----------------------------------------------------------------

SizeMessage::SizeMessage() : Message(SIZES), size(0) {
    MSG_SIZE = sizeof(size_t);
}

SizeMessage::SizeMessage(std::vector<int> pSize) : Message(SIZES), size(pSize.size()) {
    MSG_SIZE = sizeof(size_t) + sizeof(float) * size;
    size_ = pSize;
}

void SizeMessage::to_bin(){
    alloc_data(MAX_SIZE);
    memset(_data, 0, MSG_SIZE);

    char * tmp = _data;
    memcpy(tmp, &type, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(tmp, &size, sizeof(size_t));
    tmp += sizeof(size_t);

    for (auto &s : size_) {
        memcpy(tmp, &s, sizeof(float));
        tmp += sizeof(float);
    }

}

int SizeMessage::from_bin(char * src){
    alloc_data(MAX_SIZE);
    char * tmp = src;
    memcpy(static_cast<void*>(_data), tmp, MAX_SIZE);
    memcpy(&type, tmp, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(&size, tmp, sizeof(size_t));
    tmp += sizeof(size_t);

    for (size_t i = 0; i < size; ++i) {
        float s;
        memcpy(&s, tmp, sizeof(float));
        tmp += sizeof(float);
        size_.push_back(s);
    }

    return 0;
}

// FoodMessage ----------------------------------------------------------------

FoodMessage::FoodMessage() : Message(FOOD), size(0) {
    MSG_SIZE = sizeof(size_t);
}

FoodMessage::FoodMessage(std::vector<std::pair<float, float>> pFood) : Message(FOOD), size(pFood.size()) {
    MSG_SIZE = sizeof(size_t) + sizeof(float) * 2 * size;
    food = pFood;
}

void FoodMessage::to_bin(){
    alloc_data(MAX_SIZE);
    memset(_data, 0, MSG_SIZE);

    char * tmp = _data;
    memcpy(tmp, &type, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(tmp, &size, sizeof(size_t));
    tmp += sizeof(size_t);

    for (auto &p : food) {
        memcpy(tmp, &p.first, sizeof(float));
        tmp += sizeof(float);
        memcpy(tmp, &p.second, sizeof(float));
        tmp += sizeof(float);
    }
}

int FoodMessage::from_bin(char * src){
    alloc_data(MAX_SIZE);
    char * tmp = src;
    memcpy(static_cast<void*>(_data), tmp, MAX_SIZE);
    memcpy(&type, tmp, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(&size, tmp, sizeof(size_t));
    tmp += sizeof(size_t);

    for (size_t i = 0; i < size; ++i) {
        float x, y;
        memcpy(&x, tmp, sizeof(float));
        tmp += sizeof(float);
        memcpy(&y, tmp, sizeof(float));
        tmp += sizeof(float);
        food.push_back(std::make_pair(x, y));
    }

    return 0;
}

// InputMessage ---------------------------------------------------------------

InputMessage::InputMessage() : Message(INPUT), size(0) {
    MSG_SIZE = sizeof(size_t);
}

InputMessage::InputMessage(std::pair<float, float> pInput) : Message(INPUT), size(pInput.size()) {
    MSG_SIZE = sizeof(size_t) + sizeof(float) * 2 * size;
    input = pInput;
}

void InputMessage::to_bin(){
    alloc_data(MAX_SIZE);
    memset(_data, 0, MSG_SIZE);

    char * tmp = _data;
    memcpy(tmp, &type, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(tmp, &size, sizeof(size_t));
    tmp += sizeof(size_t);

    memcpy(tmp, &input.first, sizeof(float));
    tmp += sizeof(float);
    memcpy(tmp, &input.second, sizeof(float));
    tmp += sizeof(float);
}

int InputMessage::from_bin(char * src){
    alloc_data(MAX_SIZE);
    char * tmp = src;
    memcpy(static_cast<void*>(_data), tmp, MAX_SIZE);
    memcpy(&type, tmp, sizeof(MessageType));
    tmp += sizeof(MessageType);

    memcpy(&size, tmp, sizeof(size_t));
    tmp += sizeof(size_t);

    memcpy(&input.first, tmp, sizeof(float));
    tmp += sizeof(float);
    memcpy(&input.second, tmp, sizeof(float));
    tmp += sizeof(float);

    return 0;
}
