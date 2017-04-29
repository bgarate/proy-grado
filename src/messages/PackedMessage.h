//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_PACKEDMESSAGE_H
#define PROY_GRADO_PACKEDMESSAGE_H

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include "vector"

typedef std::vector<boost::uint8_t> data_buffer;

template <class T>
class PackedMessage {

public:
    static const unsigned HEADER_SIZE = 4;
    typedef boost::shared_ptr<T> MessagePointer;

    PackedMessage(MessagePointer msg = MessagePointer(new T))
            : msg(msg) {

    }

    void setMsg(MessagePointer msg)
    {
        this->msg = msg;
    }

    MessagePointer getMsg()
    {
        return msg;
    }

    bool pack(data_buffer& buf) const
    {
        if (!msg)
            return false;

        unsigned msg_size = msg->ByteSize();
        buf.resize(HEADER_SIZE + msg_size);
        encode_header(buf, msg_size);
        return msg->SerializeToArray(&buf[HEADER_SIZE], msg_size);
    }

    static unsigned decode_header(const data_buffer& buf)
    {
        if (buf.size() < HEADER_SIZE)
            return 0;
        unsigned msg_size = 0;
        for (unsigned i = 0; i < HEADER_SIZE; ++i)
            msg_size = msg_size * 256 + (static_cast<unsigned>(buf[i]) & 0xFF);
        return msg_size;
    }

    bool unpack(const data_buffer& buf) {
        return msg->ParseFromArray(&buf[HEADER_SIZE], buf.size() - HEADER_SIZE);
    }

private:
    MessagePointer msg;

    void encode_header(data_buffer& buf, unsigned size) const
    {
        assert(buf.size() >= HEADER_SIZE);
        buf[0] = static_cast<boost::uint8_t>((size >> 24) & 0xFF);
        buf[1] = static_cast<boost::uint8_t>((size >> 16) & 0xFF);
        buf[2] = static_cast<boost::uint8_t>((size >> 8) & 0xFF);
        buf[3] = static_cast<boost::uint8_t>(size & 0xFF);
    }

};


#endif //PROY_GRADO_PACKEDMESSAGE_H
