//
// Created by bruno on 10/04/17.
//

#include "MessageBuilder.h"

Message MessageBuilder::build(Message_Type type) {

    Message msg;
    msg.set_type(type);

    switch (type){
        case Message_Type_PING: {
            Ping* ping = new Ping();
            msg.set_allocated_ping(ping);
            break;
        }
        case Message_Type_ADVERTISEMENT: {
            Advertisement* advertisement = new Advertisement();
            msg.set_allocated_advertisement(advertisement);
            break;
        }
        case Message_Type_SHUTDOWN: {
            DoShutdown* shutdown = new DoShutdown();
            msg.set_allocated_shutdown(shutdown);
            break;
        }
        default:
            throw new std::runtime_error("Message type not recognized");
    }

    return msg;

}
