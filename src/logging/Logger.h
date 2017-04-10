//
// Created by bruno on 02/04/17.
//

#ifndef PROY_GRADO_LOGGER_H
#define PROY_GRADO_LOGGER_H

#include "spdlog/logger.h"
#include "string"

class Logger {

public:
    Logger();
    ~Logger();

    static Logger& getInstance();
    enum class LogType { CRITICAL, ERROR, WARNING, INFO, DEBUG};
    void Log(std::string msg, LogType severity);

    static void logCritical(std::string msg);
    static void logError(std::string msg);
    static void logWarning(std::string msg);
    static void logInfo(std::string msg);
    static void logDebug(std::string msg);

    void setSource(std::string source);

private:
    std::shared_ptr<spdlog::logger> console;
    std::string source;
    void initConsole(std::string name);
};


#endif //PROY_GRADO_LOGGER_H
