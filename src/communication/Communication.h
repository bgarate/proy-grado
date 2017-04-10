//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_COMMUNICATION_H
#define PROY_GRADO_COMMUNICATION_H

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "iostream"

using boost::asio::ip::tcp;
using boost::asio::io_service;

class Communication {

public:
    ~Communication();
    bool isServer();
    void connect(std::string host, unsigned short port);
    void serve(unsigned short  port);

    Message receive();
    void send(Message msg);

private:
    bool isServing;
    io_service service;
    tcp::socket* socket;

    std::vector<uint8_t> writeBuffer;
    std::vector<uint8_t> readBuffer;
};


#endif //PROY_GRADO_COMMUNICATION_H
