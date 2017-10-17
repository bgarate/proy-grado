//
// Created by bruno on 09/04/17.
//

#include <google/protobuf/message.h>
#include "../proto/message.pb.h"
//#include "../communication/Communication.h"
#include "AsyncSocketChannel.h"

#include "boost/asio/connect.hpp"
#include "boost/asio/write.hpp"
#include "boost/asio/read.hpp"
#include "boost/bind.hpp"
#include "../logging/Logger.h"
#include "boost/asio/placeholders.hpp"
#include "Connection.h"

namespace asio = boost::asio;

Connection::Pointer AsyncSocketChannel::tryConnect(boost::asio::ip::address_v4 address, unsigned short port){

    Logger::logDebug("Connecting to %s:%u") << address.to_string() << port;

    Connection::Pointer connection = Connection::create(service, queue);

    tcp::endpoint endpoint = tcp::endpoint(address, port);

    connection->getSocket().connect(endpoint);

    return connection;

}

Connection::Pointer AsyncSocketChannel::connect(boost::asio::ip::address_v4 address, unsigned short port) {

    int retries = 0;
    bool success = false;

    Connection::Pointer res;

    while (!success) {
        try {
            res = tryConnect(address, port);
            success = true;
        } catch (const boost::system::system_error &ex) {
            if (retries >= MAX_NUMBER_RETRIES) {
                Logger::logError("Couldn't connect. Maximum number of retries reached");
                throw;
            }
            retries++;
            Logger::logWarning("Couldn't connect. Retry %u of %u") << retries,MAX_NUMBER_RETRIES;
            sleep(1);
        }
    }

    return res;

}

bool AsyncSocketChannel::isServer() {
    return isServing;
}

void AsyncSocketChannel::serve(unsigned short port) {

    Logger::logDebug("Starting server on %s") << port;

    acceptor = boost::shared_ptr<tcp::acceptor>(new tcp::acceptor(service, tcp::endpoint(tcp::v4(), port)));

    isServing = true;
    servingPort = port;

    acceptNext();

}

void AsyncSocketChannel::acceptNext() {

    Connection::Pointer connection = Connection::create(acceptor->get_io_service(), queue);

    acceptor->async_accept(connection->getSocket(), [this, connection](const boost::system::error_code& error){

        if(!error){
            connection->start();
            connections.push_back(connection);
            AceptedConnection(connection);
        } else {
            Logger::logError("Error accepting connection:\n%s") << error.message();
        }

        acceptNext();

    });

}

AsyncSocketChannel::~AsyncSocketChannel() {

    for(boost::shared_ptr<Connection> connection : connections)
    if(connection->getSocket().is_open()){
        connection->getSocket().close();
    }

}

AsyncSocketChannel::AsyncSocketChannel(MessageQueue* queue):queue(queue) {

}

Connection::Pointer AsyncSocketChannel::getConnection(boost::asio::ip::address_v4 address, unsigned short port) {

    for(Connection::Pointer c: connections){
        boost::asio::ip::tcp::endpoint remoteEndpoint = c->getSocket().remote_endpoint();

        if(remoteEndpoint.address().to_v4() == address && remoteEndpoint.port() == port){
            return c;
        }
    }

    return NULL;
}

unsigned short AsyncSocketChannel::getServingPort() {
    return servingPort;
}


