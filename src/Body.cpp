//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include "Body.h"
#include "logging/Logger.h"


void Body::communicate(std::string brainHost, unsigned short port) {
    communication.connect(brainHost, port);
    Logger::logInfo("Body has established a connection!");

    Message msg = communication.receive();
    Logger::logInfo("PING REQUEST received");
}

