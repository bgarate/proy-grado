//
// Created by bruno on 10/04/17.
//

#ifndef PROY_GRADO_MESSAGEBUILDER_H
#define PROY_GRADO_MESSAGEBUILDER_H


#include <src/proto/message.pb.h>
#include "boost/asio/ip/address_v4.hpp"


class MessageBuilder {
public:
    static Message build(Message_Type type);
    static Message hello(std::string name, unsigned int id, boost::asio::ip::address_v4 ip, unsigned short port);
};


#endif //PROY_GRADO_MESSAGEBUILDER_H
