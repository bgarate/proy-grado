//
// Created by bruno on 27/04/17.
//

#include "Broadcaster.h"
#include "PackedMessage.h"

#include <boost/asio/write.hpp>

namespace asio = boost::asio;

void Broadcaster::setup(unsigned short port) {
    socket = new udp::socket(service);

    socket->open(udp::v4());
    socket->set_option(udp::socket::reuse_address(true));
    socket->set_option(asio::socket_base::broadcast(true));

    endpoint = udp::endpoint(asio::ip::address_v4::broadcast(), port);
    this->port = port;
}

void Broadcaster::broadcast(Message msg) {

    boost::shared_ptr<Message> ptr(new Message(msg));
    PackedMessage<Message> packedMessage(ptr);

    writeBuffer.clear();
    packedMessage.pack(writeBuffer);

    socket->send_to(asio::buffer(writeBuffer), endpoint);

}

Message Broadcaster::receive() {

    if(!binded) {
        socket->bind(udp::endpoint(asio::ip::address_v4::any(),port));
    }
    readBuffer.resize(PackedMessage<Message>::HEADER_SIZE);
    socket->receive_from(asio::buffer(readBuffer), endpoint);

    unsigned messageLength = PackedMessage<Message>::decode_header(readBuffer);

    readBuffer.resize(PackedMessage<Message>::HEADER_SIZE + messageLength);

    asio::mutable_buffers_1 buffer = asio::buffer(&readBuffer[PackedMessage<Message>::HEADER_SIZE], messageLength);

    socket->receive_from(buffer, endpoint);

    PackedMessage<Message> packedMessage;
    packedMessage.unpack(readBuffer);

    return *packedMessage.getMsg();

}

Broadcaster::~Broadcaster() {
    if(socket->is_open()){
        socket->close();
    }
    delete socket;
}
