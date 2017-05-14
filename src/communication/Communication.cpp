//
// Created by bruno on 27/04/17.
//

#include <src/messages/MessageBuilder.h>
#include "Communication.h"

Communication::Communication() :
        queue(new MessageQueue), socketHandler(queue) {


}

void Communication::setup(std::string name, unsigned int id) {

    this->name = name;
    this->id = id;
}

Communication::~Communication() {
    delete queue;
}

void Communication::serve(unsigned short port) {

    socketHandler.AceptedConnection = [this](Connection::Pointer c){acceptedConnectionHandler(c); };

    socketHandler.serve(port);

}

void Communication::acceptedConnectionHandler(Connection::Pointer connection) {

    boost::asio::ip::tcp::endpoint localEndpoint = connection->getSocket().local_endpoint();
    Message msg = MessageBuilder::hello(name, id, localEndpoint.address().to_v4(), localEndpoint.port());

    connection->send(msg);

}

void Communication::connect(address_v4 address, unsigned short port) {

    Connection::Pointer c = socketHandler.getConnection(address, port);

    if(c != NULL)
        return;

    Connection::Pointer connection = socketHandler.connect(address, port);
    boost::asio::ip::tcp::endpoint localEndpoint = connection->getSocket().local_endpoint();
    Message msg = MessageBuilder::hello(name, id, localEndpoint.address().to_v4(), localEndpoint.port());

}

void Communication::asociate(Hello* hello) {

    Connection::Pointer c = socketHandler.getConnection(boost::asio::ip::address_v4(hello->ip()),
                                                        (unsigned short) hello->port());

    if(c == NULL)
        throw new std::runtime_error("Couldn't find a matching remote endpoint to asociate a connection");

    connections[hello->id()] = c;

}

Message Communication::getMessage() {
    return queue->pop();
}

bool Communication::messageAvailable() {
    return !queue->empty();
}
