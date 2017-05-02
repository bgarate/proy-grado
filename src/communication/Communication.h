//
// Created by bruno on 27/04/17.
//

#ifndef PROY_GRADO_COMMUNICATION_H
#define PROY_GRADO_COMMUNICATION_H

#include "boost/asio/ip/address_v4.hpp"

using boost::asio::ip::address_v4;

class Communication {
public:
    void serve();
    void add(address_v4 address, unsigned short port);
};


#endif //PROY_GRADO_COMMUNICATION_H
