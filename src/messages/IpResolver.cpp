//
// Created by bruno on 15/05/17.
//

#include <ifaddrs.h>
#include "IpResolver.h"


boost::asio::ip::address_v4 IpResolver::resolve_ip(bool includeLoopback, std::string interface) {
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa = NULL;
    in_addr tmpAddrPtr;

    getifaddrs(&ifAddrStruct);

    boost::asio::ip::address_v4 address;

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {

        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) {
            continue;
        }

        tmpAddrPtr= ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

        if((includeLoopback || std::string(ifa->ifa_name).substr(0,2) != "lo") &&
                (interface == "" || interface == std::string(ifa->ifa_name))) {

            boost::asio::ip::address_v4::bytes_type bytes;

            bytes[0] = (unsigned char) (tmpAddrPtr.s_addr & 0xFF);
            bytes[1] = (unsigned char) ((tmpAddrPtr.s_addr >> 8) & 0xFF);
            bytes[2] = (unsigned char) ((tmpAddrPtr.s_addr >> 16) & 0xFF);
            bytes[3] = (unsigned char) ((tmpAddrPtr.s_addr >> 24) & 0xFF);

            address = boost::asio::ip::address_v4(bytes);

            break;

        }

    }

    if (ifAddrStruct != NULL)
        freeifaddrs(ifAddrStruct);

    return address;

}

boost::asio::ip::address_v4 IpResolver::resolve_broadcast(bool includeLoopback, std::string interface) {
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa = NULL;
    in_addr tmpAddrPtr;

    getifaddrs(&ifAddrStruct);

    boost::asio::ip::address_v4 address;

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {

        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) {
            continue;
        }

        tmpAddrPtr= ((struct sockaddr_in *)ifa->ifa_ifu.ifu_broadaddr)->sin_addr;

        if((includeLoopback || std::string(ifa->ifa_name).substr(0,2) != "lo") &&
           (interface == "" || interface == std::string(ifa->ifa_name))) {

            boost::asio::ip::address_v4::bytes_type bytes;

            bytes[0] = (unsigned char) (tmpAddrPtr.s_addr & 0xFF);
            bytes[1] = (unsigned char) ((tmpAddrPtr.s_addr >> 8) & 0xFF);
            bytes[2] = (unsigned char) ((tmpAddrPtr.s_addr >> 16) & 0xFF);
            bytes[3] = (unsigned char) ((tmpAddrPtr.s_addr >> 24) & 0xFF);

            address = boost::asio::ip::address_v4(bytes);

            break;

        }

    }

    if (ifAddrStruct != NULL)
        freeifaddrs(ifAddrStruct);

    return address;

}
