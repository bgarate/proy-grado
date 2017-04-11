//
// Created by bruno on 10/04/17.
//

#include "MessageBuilder.h"

Message& MessageBuilder::build(Message_Type type) {

    Message msg;
    msg.set_type(type);

    switch (type){
        case Message_Type_PING:
            Ping ping;
            msg.set_allocated_ping(&ping);
            break;
        default:
            throw;
    }

    return msg;

}
