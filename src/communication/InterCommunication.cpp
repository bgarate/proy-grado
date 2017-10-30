//
// Created by santy on 15/10/17.
//

#include "../messages/IpResolver.h"
#include "../messages/MessageBuilder.h"
#include "../config/ConfigKeys.h"
#include "InterCommunication.h"

//Public

InterCommunication::InterCommunication() : queue(new MessageQueue), socketHandler(queue) {

    messsageHandler.registerHandler(Message_Type::Message_Type_ADVERTISEMENT,
                                    [this](Message m){this->advertisementHandler(m);});
    messsageHandler.registerHandler(Message_Type::Message_Type_HELLO,
                                    [this](Message m){this->helloHandler(m);});
}

void InterCommunication::setupInterComm(Config* config){

    advertisementLapse = config->Get(ConfigKeys::Communications::AdvertisementLapse);

    broadcaster.setup(config->Get(ConfigKeys::Communications::BroadcastPort));

    this->name = config->Get(ConfigKeys::Drone::Name);
    this->id = config->Get(ConfigKeys::Drone::Id);
    socketPort = config->Get(ConfigKeys::Communications::CommunicationPort);

    IpResolver resolver;
    ip = resolver.resolve();

    socketHandler.AceptedConnection = [this](Connection::Pointer c){acceptedConnectionHandler(c); };
    socketHandler.serve(socketPort);
}

void InterCommunication::interCommStep(long runningTime, long deltaTime) {

    handleMessages();
    advertise(runningTime);
}

void InterCommunication::shutdownInterComm() {

}

//Private

void InterCommunication::acceptedConnectionHandler(Connection::Pointer connection) {

    boost::asio::ip::tcp::endpoint localEndpoint = connection->getSocket().local_endpoint();
    Message msg = MessageBuilder::hello(name, id, localEndpoint.address().to_v4(), localEndpoint.port());

    connection->send(msg);

}

void InterCommunication::advertisementHandler(Message& msg){

    Advertisement* advertisement = msg.mutable_advertisement();

    boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(advertisement->ip());

    if(address != ip || advertisement->port() != socketPort){
        Logger::logDebug("Advertisement received from %s:%u") << address.to_string() <<
                                                              advertisement->port();

        Connection::Pointer c = socketHandler.getConnection(address, advertisement->port());

        if(c != NULL)
            return;

        Connection::Pointer connection = socketHandler.connect(address, advertisement->port());
        boost::asio::ip::tcp::endpoint localEndpoint = connection->getSocket().local_endpoint();
        Message msg = MessageBuilder::hello(name, id, localEndpoint.address().to_v4(), localEndpoint.port());
        connection->send(msg);
    }

}

void InterCommunication::helloHandler(Message& msg){

    Hello* hello = msg.mutable_hello();

    boost::asio::ip::address_v4 address = boost::asio::ip::address_v4(hello->ip());

    Logger::logDebug("Hello received from %s(%u) at %s:%u") << hello->name() <<
                                                            hello->id() << address.to_string() <<
                                                            hello->port();

    Connection::Pointer c = socketHandler.getConnection(boost::asio::ip::address_v4(hello->ip()),
                                                        (unsigned short) hello->port());

    if(c == NULL)
        throw new std::runtime_error("Couldn't find a matching remote endpoint to asociate a connection");

    connections[hello->id()] = c;

}


void InterCommunication::handleMessages(){

    if(broadcaster.messageAvailable()) {
        Message msg = broadcaster.receive();
        messsageHandler.handle(msg);
    }

    while (!queue->empty()) {
        Message msg = queue->pop();
        messsageHandler.handle(msg);
    }
}

void InterCommunication::advertise(long runningTime) {

    if(runningTime - lastAdvertisementTime > advertisementLapse * 1000) {

        Message msg = MessageBuilder::build(Message_Type::Message_Type_ADVERTISEMENT);

        Advertisement* advertisement = msg.mutable_advertisement();

        advertisement->set_ip((uint32_t) ip.to_ulong());
        advertisement->set_port(socketPort);

        broadcaster.broadcast(msg);
        Logger::logDebug("Advertising sent");

        lastAdvertisementTime = runningTime;
    }
}


