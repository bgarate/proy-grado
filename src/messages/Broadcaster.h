//
// Created by bruno on 27/04/17.
//

#ifndef PROY_GRADO_BROADCASTER_H
#define PROY_GRADO_BROADCASTER_H

#include <boost/asio/ip/udp.hpp>
#include <src/proto/message.pb.h>

using boost::asio::ip::udp;
using boost::asio::io_service;

class Broadcaster {

public:
    ~Broadcaster();
    void broadcast(Message msg);
    void setup(unsigned short port);
    Message receive();

    bool messageAvailable();
private:
    io_service service;
    udp::socket* socket;

    udp::endpoint endpoint;

    std::vector<uint8_t> writeBuffer;
    std::vector<uint8_t> readBuffer;

    bool binded = false;
    unsigned short port;

};


#endif //PROY_GRADO_BROADCASTER_H
