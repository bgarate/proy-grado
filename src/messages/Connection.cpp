//
// Created by bruno on 13/05/17.
//

#include "boost/asio/placeholders.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "boost/asio/read.hpp"
#include "boost/asio/write.hpp"
#include "boost/asio/connect.hpp"
#include <src/logging/Logger.h>
#include <boost/bind.hpp>
#include "Connection.h"
#include "PackedMessage.h"

namespace asio = boost::asio;
using boost::asio::ip::tcp;
using boost::asio::io_service;

Connection::Connection(io_service &io_service, MessageQueue* queue): socket(io_service)
{
    this->queue = queue;
}

tcp::socket& Connection::getSocket(){
    return socket;
}

void Connection::start() {
    const boost::asio::ip::basic_endpoint <tcp> remote = socket.remote_endpoint();
    Logger::logDebug("Connected with %s:%u") << remote.address() << remote.port();

    receive();

}

Connection::Pointer Connection::create(boost::asio::io_service& io_service, MessageQueue* queue) {
    return Connection::Pointer(new Connection(io_service, queue));
};;

void Connection::receive() {

    readBuffer.resize(PackedMessage<Message>::HEADER_SIZE);
    asio::async_read(socket, boost::asio::buffer(readBuffer),
                     boost::bind(&Connection::handleReadHeader, shared_from_this(), boost::asio::placeholders::error));

}

void Connection::handleReadBody(const boost::system::error_code& error){

    if(!error) {
        PackedMessage<Message> packedMessage;
        packedMessage.unpack(readBuffer);

        queue->push(*packedMessage.getMsg());

        receive();
    } else {
        Logger::logError("Error while receiving body:\n%s" + error.message());
    }

}

asio::ip::address_v4 Connection::getIp() {
    return socket.local_endpoint().address().to_v4();
}

unsigned short Connection::getPort() {
    return socket.local_endpoint().port();
}


void Connection::handleReadHeader(const boost::system::error_code& error){

    if(!error){

        unsigned messageLength = PackedMessage<Message>::decode_header(readBuffer);
        readBuffer.resize(PackedMessage<Message>::HEADER_SIZE + messageLength);
        boost::asio::mutable_buffers_1 buffer = asio::buffer(
                &readBuffer[PackedMessage<Message>::HEADER_SIZE], messageLength);

        async_read(socket, buffer,
                         boost::bind(&Connection::handleReadBody, shared_from_this(), boost::asio::placeholders::error));

    } else {
        Logger::logError("Error while receiving header:\n%s") << error.message();
    }

}

void Connection::send(Message msg) {

    boost::shared_ptr<Message> ptr(new Message(msg));
    PackedMessage<Message> packedMessage(ptr);

    writeBuffer.clear();
    packedMessage.pack(writeBuffer);
    asio::write(socket, boost::asio::buffer(writeBuffer));

}