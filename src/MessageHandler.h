//
// Created by bruno on 10/04/17.
//

#ifndef PROY_GRADO_MESSAGEHANDLER_H
#define PROY_GRADO_MESSAGEHANDLER_H

#include <typeindex>
#include <boost/function.hpp>
#include <map>
#include "proto/message.pb.h"

class MessageHandler {

        typedef std::pair<Message_Type,std::function<void (Message)>> HandlerPair;

public:
    void registerHandler(Message_Type type, std::function<void(Message)> fn);
    void handle(Message& msg);

private:
    std::map<Message_Type,std::function<void (Message)>> handlers;
};


#endif //PROY_GRADO_MESSAGEHANDLER_H
