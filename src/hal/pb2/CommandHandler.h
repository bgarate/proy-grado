//
// Created by bruno on 22/05/17.
//

#ifndef PROY_GRADO_COMMANDHANDLER_H
#define PROY_GRADO_COMMANDHANDLER_H


#include <map>
#include <functional>

//sdk
extern "C"{
#include <libARSAL/ARSAL.h>
#include <libARController/ARController.h>
#include <libARDiscovery/ARDiscovery.h>
}


class CommandDictionary {
public:
    CommandDictionary(ARCONTROLLER_DICTIONARY_ELEMENT_t* dictionary);
    float getFloat(const char* str);
    double getDouble(const char* str);
    int32_t getInteger(const char *str);
    uint8_t getUInt8(const char *str);
private:
    ARCONTROLLER_DICTIONARY_ELEMENT_t *dictionary;


};

class CommandHandler {
public:
    void registerHandler(eARCONTROLLER_DICTIONARY_KEY type, std::function<void(CommandDictionary*)> fn);
    void handle(eARCONTROLLER_DICTIONARY_KEY type, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary);
private:
    std::map<eARCONTROLLER_DICTIONARY_KEY,std::function<void (CommandDictionary*)>> handlers;

};

#endif //PROY_GRADO_COMMANDHANDLER_H
