//
// Created by bruno on 30/09/17.
//

#ifndef PROY_GRADO_BODYTESTREGISTRY_H
#define PROY_GRADO_BODYTESTREGISTRY_H


#include "BodyTest.h"

#define REGISTER_BODY_TEST(NAME) Register(#NAME, []()-> BodyTest* {    \
    return new NAME();                                                                       \
})

class BodyTestRegistry {
public:
    typedef std::function<BodyTest*(void)> Handler;
    static BodyTest* Get(std::string bodyTest);
    static std::vector<std::string> GetAvailableTests();
private:
    static bool initialized;
    static void Initialize();
    static std::map<std::string, Handler> bodyTests;
    static void Register(std::string name, Handler instanceCreator);
};


#endif //PROY_GRADO_BODYTESTREGISTRY_H
