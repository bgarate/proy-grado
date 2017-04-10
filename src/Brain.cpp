//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include "Brain.h"
#include "logging/Logger.h"

void Brain::communicate(unsigned short port) {
    communication.serve(port);
    Logger::logInfo("Brain has established a connection!");

    Message msg;
    msg.set_type(Message_Type::Message_Type_PING);

    Ping ping;
    ping.set_type(Ping_PingType::Ping_PingType_REQUEST);

    msg.set_allocated_ping(&ping);

    communication.send(msg);

    Logger::logInfo("PING REQUEST sent");

}
