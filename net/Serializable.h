#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include <stdlib.h>


// Must be implemented by whatever class we want to send through the network.
// These classes must expand upon the to_bin() and from_bin() methods, which
// will be used to serialize and deserialize the class for network transmission.

class Serializable
{
public:

    Serializable():_size(0), _data(0){};

    virtual ~Serializable()
    {
        if ( _data != 0 )
        {
            free(_data);
            _data = nullptr;
        }
    }


    // Generate binary representation of the class. Must initialize internal
    // buffer with alloc_data.
    virtual void to_bin() = 0;


    // Rebuid the class from a binary representation. Will return -1 if it fails
    virtual int from_bin(char * data) = 0;


    // Return a pointer to the internal buffer with the serialized object.
    // Must be previously initialized with the to_bin() method.
    char * data() const
    {
        return _data;
    }

    
    // Return size of the serialized object.
    int32_t size()
    {
        return _size;
    }

protected:

    int32_t _size;

    char *  _data;

    
    // Memory allocation for the internal buffer.
    void alloc_data(int32_t data_size)
    {
        if ( _data != 0 )
        {
            free(_data);
        }

        _data = (char *) malloc(data_size);
        _size = data_size;
    }
};


#endif