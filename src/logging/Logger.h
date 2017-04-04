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

    static void LogCritical(std::string msg);
    static void LogError(std::string msg);
    static void LogWarning(std::string msg);
    static void LogInfo(std::string msg);
    static void LogDebug(std::string msg);


private:
    std::shared_ptr<spdlog::logger> console;

};


#endif //PROY_GRADO_LOGGER_H
