//
// Created by bruno on 10/04/17.
//

#include "MessageHandler.h"

void MessageHandler::registerHandler(Message_Type type, std::function<void(Message)> fn) {

    handlers.insert(HandlerPair(type, fn));

}

void MessageHandler::handle(Message &msg) {

    std::function<void(Message)> fn = handlers.at(msg.type());

    fn(msg);

}
