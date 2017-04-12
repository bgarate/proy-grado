//
// Created by bruno on 10/04/17.
//

#ifndef PROY_GRADO_MESSAGEHANDLER_H
#define PROY_GRADO_MESSAGEHANDLER_H

#include <typeindex>
#include <boost/function.hpp>
#include <map>

class MessageHandler {

    typedef std::pair<std::type_index,std::function> handlerPair;

public:
    template <class T>
    void registerHandler(std::function<void(T)> fn);

private:
    std::map<std::type_index,std::function> handlers;
};

template<class T>
void MessageHandler::registerHandler(std::function<void(T)> fn) {
    handlers.insert(handlerPair(std::type_index(typeid(T)),fn));
}


#endif //PROY_GRADO_MESSAGEHANDLER_H
