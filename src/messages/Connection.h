//
// Created by bruno on 13/05/17.
//

#ifndef PROY_GRADO_CONNECTION_H
#define PROY_GRADO_CONNECTION_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <src/proto/message.pb.h>
#include <boost/lockfree/queue.hpp>
#include <cstdint>
#include "iostream"
#include "PackedMessage.h"
#include "MessageQueue.h"

class Connection: public boost::enable_shared_from_this<Connection> {
public:

    typedef boost::shared_ptr<Connection> Pointer;

    static Pointer create(boost::asio::io_service& io_service, MessageQueue* queue);
    void start();

    void send(Message msg);

    boost::asio::ip::address_v4 getIp();

    unsigned short getPort();

    boost::asio::ip::tcp::socket & getSocket();

private:
    boost::asio::ip::tcp::socket socket;

    std::vector<uint8_t> writeBuffer;
    std::vector<uint8_t> readBuffer;

    Connection(boost::asio::io_service &io_service, MessageQueue* queue);

    void handleReadHeader(const boost::system::error_code& error);
    void handleReadBody(const boost::system::error_code& error);

    void receive();

    MessageQueue* queue;

};


#endif //PROY_GRADO_CONNECTION_H
