//
// Created by santy on 15/10/17.
//

#ifndef PROY_GRADO_INTERCOMMUNICATION_H
#define PROY_GRADO_INTERCOMMUNICATION_H


#include "../messages/Connection.h"
#include "../messages/AsyncSocketChannel.h"
#include "../config/Config.h"
#include "../messages/Broadcaster.h"
#include "../messages/SocketChannel.h"
#include "../messages/MessageHandler.h"

class InterCommunication {

public:

    InterCommunication();

    void setupInterComm(Config* config);

    void interCommStep(long runningTime, long deltaTime);

    void shutdownInterComm();

private:

    std::map<unsigned int, Connection::Pointer> connections;
    MessageQueue* queue;
    AsyncSocketChannel socketHandler;
    std::string name;
    unsigned int id;
    int socketPort;
    boost::asio::ip::address_v4 ip;

    void acceptedConnectionHandler(Connection::Pointer connection);

    //Communication bodySocket;
    MessageHandler messsageHandler;
    long lastAdvertisementTime = 0;
    Broadcaster broadcaster;
    int advertisementLapse;


    void advertisementHandler(Message &msg);

    void helloHandler(Message &msg);

    void advertise(long runningTime);

    void handleMessages();


};


#endif //PROY_GRADO_INTERCOMMUNICATION_H
