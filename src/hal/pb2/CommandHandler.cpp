//
// Created by bruno on 22/05/17.
//

#include <libARController/ARCONTROLLER_DICTIONARY_Key.h>
#include <libARController/ARCONTROLLER_Dictionary.h>
#include <libARController/ARController.h>
#include "CommandHandler.h"


void CommandHandler::registerHandler(eARCONTROLLER_DICTIONARY_KEY type,
                                     std::function<void(CommandDictionary*)> fn) {

    handlers[type] = fn;

}

void CommandHandler::handle(eARCONTROLLER_DICTIONARY_KEY type, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary) {

    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
    HASH_FIND_STR (elementDictionary, ARCONTROLLER_DICTIONARY_SINGLE_KEY, element);

    if (element != NULL) {

        std::function<void(CommandDictionary*)> &fn= handlers[type];
        CommandDictionary* commandDictionary = new CommandDictionary(element);
        fn(commandDictionary);
        delete commandDictionary;
    }

}

CommandDictionary::CommandDictionary(ARCONTROLLER_DICTIONARY_ELEMENT_t *dictionary) {
    this->dictionary = dictionary;
}

float CommandDictionary::getFloat(const char *str) {
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    HASH_FIND_STR (dictionary, str, arg);
    return arg->value.Float;
}

double CommandDictionary::getDouble(const char *str) {
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    HASH_FIND_STR (dictionary, str, arg);
    return arg->value.Double;
}

int32_t CommandDictionary::getInteger(const char *str) {
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    HASH_FIND_STR (dictionary, str, arg);
    return arg->value.I32;
}

uint8_t CommandDictionary::getUInt8(const char *str) {
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    HASH_FIND_STR (dictionary, str, arg);
    return arg->value.U8;
}
