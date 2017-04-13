//
// Created by bruno on 10/04/17.
//

#ifndef PROY_GRADO_MESSAGEBUILDER_H
#define PROY_GRADO_MESSAGEBUILDER_H


#include <src/proto/message.pb.h>

class MessageBuilder {
public:
    static Message build(Message_Type type);

};


#endif //PROY_GRADO_MESSAGEBUILDER_H
