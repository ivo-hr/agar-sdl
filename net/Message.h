#include <vector>
#include <memory>
#include <cstring>

#include "Serializable.h"
#include "Socket.h"


enum MessageType
{
    LOGIN = 0,
    CONFIRM,
    LOGOUT,
    POSITIONS,
    SIZES,
    FOOD,
    INPUT
};

class Message : public Serializable
{
public:
    static const size_t MAX_SIZE = 4000;

    size_t MSG_SIZE = sizeof(size_t);

    Message(MessageType t);
    Message();
    Message(const Message &m);
    virtual ~Message() {};

    void to_bin() override;
    int from_bin(char * src) override;

    int type;
};

class LoginMessage : public Message
{
public:
    LoginMessage();
    LoginMessage(std::string name);

    virtual ~LoginMessage() {};

    void to_bin() override;
    int from_bin(char * src) override;

    std::string name;
    size_t size;
};

class ConfirmMessage : public Message
{
public:
    ConfirmMessage();
    ConfirmMessage(int index);

    virtual ~ConfirmMessage() {};

    void to_bin() override;
    int from_bin(char * src) override;

    int index;
    size_t size;
};

class PositionMessage : public Message
{
public:
    PositionMessage();
    PositionMessage(std::vector<std::pair<float, float>> pPos);

    virtual ~PositionMessage() {};

    void to_bin() override;
    int from_bin(char * src) override;

    std::vector<std::pair<float, float>> pos;
    size_t size;
};

class SizeMessage : public Message
{
public:
    SizeMessage();
    SizeMessage(std::vector<int> pSize);

    virtual ~SizeMessage() {};

    void to_bin() override;
    int from_bin(char * src) override;

    std::vector<int> sizes;
    size_t size;
};

class FoodMessage : public Message
{
public:
    FoodMessage();
    FoodMessage(std::vector<std::pair<float, float>> pFood);

    virtual ~FoodMessage() {};

    void to_bin() override;
    int from_bin(char * src) override;

    std::vector<std::pair<float, float>> food;
    size_t size;
};

class InputMessage : public Message
{
public:
    InputMessage();
    InputMessage(std::pair<float, float> pInput);

    virtual ~InputMessage() {};

    void to_bin() override;
    int from_bin(char * src) override;

    std::pair<float, float> input;
    size_t size;
};

class LogoutMessage : public Message
{
public:
    LogoutMessage();
    LogoutMessage(int index);

    virtual ~LogoutMessage() {};

    void to_bin() override;
    int from_bin(char * src) override;

    int index;
    size_t size;
};