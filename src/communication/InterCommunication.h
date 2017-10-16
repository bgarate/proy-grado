//
// Created by santy on 15/10/17.
//

#ifndef PROY_GRADO_INTERCOMMUNICATION_H
#define PROY_GRADO_INTERCOMMUNICATION_H


#include "../config/Config.h"
#include "../messages/Broadcaster.h"
#include "../messages/SocketChannel.h"
#include "../messages/MessageHandler.h"
#include "Communication.h"

class InterCommunication {

public:

    InterCommunication();

    void setupInterComm(Config* config);

    void interCommStep(long runningTime, long deltaTime);

    void shutdownInterComm();

private:

    Communication communication;
    SocketChannel bodyCommunication;
    MessageHandler messsageHandler;
    long lastAdvertisementTime = 0;
    Broadcaster broadcaster;
    unsigned int pingWait;
    bool waitingPing;
    int advertisementLapse;
    int pingTimeout;
    bool pingEnabled;

    //Handler para los mensajes

    void pingHandler(Message &msg);

    void advertisementHandler(Message &msg);

    void helloHandler(Message &msg);

    //

    void communicateWithBody(unsigned short port);

    void advertise(long runningTime);


    void handleMessages();


    void sendPingIfAppropiate(long deltaTime);

};


#endif //PROY_GRADO_INTERCOMMUNICATION_H
