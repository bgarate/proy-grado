//
// Created by bruno on 09/04/17.
//

#ifndef PROY_GRADO_ASYNSOCKETCHANNEL_H
#define PROY_GRADO_ASYNSOCKETCHANNEL_H

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/shared_ptr.hpp>
#include <src/proto/message.pb.h>
#include <boost/lockfree/queue.hpp>
#include "iostream"
#include "Connection.h"

using boost::asio::ip::tcp;
using boost::asio::io_service;

class AsyncSocketChannel {

public:

    AsyncSocketChannel(MessageQueue* queue);

    typedef std::function<void(Connection::Pointer)> ConnectionAceptedHandler;
    ConnectionAceptedHandler AceptedConnection;

    ~AsyncSocketChannel();
    bool isServer();
    Connection::Pointer connect(boost::asio::ip::address_v4 address, unsigned short port);
    void serve(unsigned short port);
    unsigned short getServingPort();
    Connection::Pointer getConnection(boost::asio::ip::address_v4 address, unsigned short port);
    std::vector<Connection::Pointer> connections;
private:

    bool isServing;
    io_service service;

    static const int MAX_NUMBER_RETRIES = 3;

    void acceptNext();
    unsigned short servingPort;

    boost::shared_ptr<tcp::acceptor> acceptor;

    MessageQueue* queue;

    Connection::Pointer tryConnect(boost::asio::ip::address_v4 address, unsigned short port);

};


#endif //PROY_GRADO_ASYNSOCKETCHANNEL_H
