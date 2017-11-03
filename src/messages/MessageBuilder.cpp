//
// Created by bruno on 10/04/17.
//

#include <boost/asio/ip/address_v4.hpp>
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
        case Message_Type_HELLO: {
            Hello* hello = new Hello();
            msg.set_allocated_hello(hello);
            break;
        }case Message_Type_DRONESTATE: {
            DroneState* state = new DroneState();
            msg.set_allocated_dronestate(state);
            break;
        }
        default:
            throw new std::runtime_error("Message type not recognized");
    }

    return msg;

}

Message MessageBuilder::hello(std::string name, unsigned int id, boost::asio::ip::address_v4 ip,
                              unsigned short port) {
    Message msg = build(Message_Type_HELLO);

    Hello* hello = msg.mutable_hello();

    hello->set_ip((google::protobuf::uint32) ip.to_ulong());
    hello->set_port(port);
    hello->set_name(name);
    hello->set_ip(id);

    return msg;

}
