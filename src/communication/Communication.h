//
// Created by bruno on 27/04/17.
//

#ifndef PROY_GRADO_COMMUNICATION_H
#define PROY_GRADO_COMMUNICATION_H

#include <boost/shared_ptr.hpp>
#include "../messages/AsyncSocketChannel.h"
#include "../messages/MessageQueue.h"
#include "../messages/Connection.h"
#include "boost/asio/ip/address_v4.hpp"
#include "boost/lockfree/queue.hpp"
#include "../proto/message.pb.h"

using boost::asio::ip::address_v4;

class Communication {
public:
    Communication();

    ~Communication();
    void serve(unsigned short port);
    void connect(address_v4 address, unsigned short port);
    void asociate(Hello* hello);
    bool messageAvailable();
    Message getMessage();
    void setup(std::string name, unsigned int id);
    boost::asio::ip::address_v4 getIp();
    unsigned short getPort();
private:
    std::map<unsigned int, Connection::Pointer> connections;
    MessageQueue* queue;
    AsyncSocketChannel socketHandler;
    std::string name;
    unsigned int id;
    void acceptedConnectionHandler(Connection::Pointer connection);


};


#endif //PROY_GRADO_COMMUNICATION_H

