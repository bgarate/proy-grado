//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include "SocketChannel.h"

#include "boost/asio/connect.hpp"
#include "boost/asio/buffer.hpp"
#include "boost/asio/write.hpp"
#include "boost/asio/read.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/basic_socket.hpp"
#include "../logging/Logger.h"
#include "PackedMessage.h"
#include "IpResolver.h"

namespace asio = boost::asio;

const int SocketChannel::MAX_NUMBER_RETRIES = 3;

void SocketChannel::tryConnect(std::string host, unsigned short port){

    Logger::logDebug("Connecting to %s:%u") << host << port;

    tcp::resolver resolver(service);
    tcp::resolver::query query(host, std::to_string(port));

    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    socket = new tcp::socket(service);
    asio::connect(*socket, endpoint_iterator);

    Logger::logDebug("Connected to %s:%u") << socket->remote_endpoint().address() << port;

    isServing = false;
}

void SocketChannel::connect(std::string host, unsigned short port) {

    int retries = 0;
    bool success = false;

    while (!success) {
        try {
            tryConnect(host, port);
            success = true;
        } catch (const boost::system::system_error &ex) {
            if (retries >= MAX_NUMBER_RETRIES) {
                Logger::logError("Couldn't connect. Maximum number of retries reached");
                throw;
            }
            retries++;
            Logger::logWarning("Couldn't connect. Retry %u of %u") << retries << MAX_NUMBER_RETRIES;
            sleep(1);
        }
    }

}

bool SocketChannel::isServer() {
    return isServing;
}

void SocketChannel::serve(unsigned short port) {

    IpResolver resolver;
    boost::asio::ip::address_v4 ip = resolver.resolve();
    Logger::logDebug("Starting server on %s:%u") << ip.to_string() << port;

    tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), port));

    socket = new tcp::socket(service);
    acceptor.accept(*socket);

    asio::ip::basic_endpoint<tcp> remote = socket->remote_endpoint();
    Logger::logDebug("Connected with %s:%u") << remote.address().to_string() << remote.port();

    isServing = true;

}

void SocketChannel::send(Message msg) {

    boost::shared_ptr<Message> ptr(new Message(msg));
    PackedMessage<Message> packedMessage(ptr);

    writeBuffer.clear();
    packedMessage.pack(writeBuffer);
    asio::write(*socket, asio::buffer(writeBuffer));

}

Message SocketChannel::receive() {

    readBuffer.resize(PackedMessage<Message>::HEADER_SIZE);
    asio::read(*socket, asio::buffer(readBuffer));

    unsigned messageLength = PackedMessage<Message>::decode_header(readBuffer);

    readBuffer.resize(PackedMessage<Message>::HEADER_SIZE + messageLength);

    asio::mutable_buffers_1 buffer = asio::buffer(&readBuffer[PackedMessage<Message>::HEADER_SIZE], messageLength);

    asio::read(*socket, buffer);

    PackedMessage<Message> packedMessage;
    packedMessage.unpack(readBuffer);

    return *packedMessage.getMsg();

}

bool SocketChannel::messageAvailable() {

    boost::asio::socket_base::bytes_readable command(true);
    socket->io_control(command);
    std::size_t bytes_readable = command.get();

    return bytes_readable >= PackedMessage<Message>::HEADER_SIZE;

}

SocketChannel::~SocketChannel() {
    if(socket->is_open()){
        socket->close();
    }
    delete socket;
}

asio::ip::address_v4 SocketChannel::getIp() {
    return socket->local_endpoint().address().to_v4();
}

unsigned short SocketChannel::getPort() {
    return socket->local_endpoint().port();
}
