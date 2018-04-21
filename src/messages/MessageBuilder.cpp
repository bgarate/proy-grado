//
// Created by bruno on 10/04/17.
//

#include <boost/asio/ip/address_v4.hpp>
#include "MessageBuilder.h"

Message MessageBuilder::build(Message_Type type) {

    Message msg;
    msg.set_type(type);

    switch (type){
        case Message_Type_DRONESTATE: {
            DroneState* state = new DroneState();
            msg.set_allocated_dronestate(state);
            break;
        }
        default:
            throw new std::runtime_error("Message type not recognized");
    }

    return msg;

}

