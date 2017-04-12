//
// Created by bruno on 09/04/17.
//

#include <src/proto/message.pb.h>
#include <src/communication/MessageBuilder.h>
#include "Brain.h"
#include "logging/Logger.h"

void Brain::communicate(unsigned short port) {
    communication.serve(port);
    Logger::logInfo("Brain has established a connection!");

    Message msg = MessageBuilder::build(Message_Type_PING);
    msg.mutable_ping()->set_type(Ping_PingType::Ping_PingType_REQUEST);

    communication.send(msg);

    Logger::logInfo("PING REQUEST sent");

}
