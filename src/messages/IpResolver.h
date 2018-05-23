//
// Created by bruno on 15/05/17.
//

#ifndef PROY_GRADO_IPRESOLVER_H
#define PROY_GRADO_IPRESOLVER_H

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include "boost/asio/ip/address_v4.hpp"

class IpResolver {
public:
    boost::asio::ip::address_v4 resolve_ip(bool includeLoopback = false, std::string interface = "");

    boost::asio::ip::address_v4 resolve_broadcast(bool includeLoopback = false, std::string interface = "");
};


#endif //PROY_GRADO_IPRESOLVER_H
