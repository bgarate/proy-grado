//
// Created by bruno on 02/04/17.
//

#ifndef PROY_GRADO_LOGGER_H
#define PROY_GRADO_LOGGER_H

#include <boost/format.hpp>
#include "spdlog/logger.h"
#include "string"

class Logger {

public:

    enum class LogType { CRITICAL, ERROR, WARNING, INFO, DEBUG};

    class Formatter {
    public:

        Formatter(std::string msg, LogType logType):format(msg), logType(logType) {
            if(format.remaining_args() == 0)
                Logger::getInstance().Log(format.str(),logType);
        }

        ~Formatter() {
            if(format.remaining_args() > 0)
                throw new std::runtime_error("Formatter needed");
        }

        template<class T>
        Logger::Formatter &operator<<(const T &x) {
            format % x;

            if(format.remaining_args() == 0)
                Logger::getInstance().Log(format.str(),logType);

            return *this;
        }

    private:
        LogType logType;
        boost::format format;
    };

    Logger();
    ~Logger();

    static Logger& getInstance();

    void Log(std::string msg, LogType severity);

    static Formatter logDebug(std::string msg);
    static Formatter logInfo(std::string msg);
    static Formatter logCritical(std::string msg);
    static Formatter logError(std::string msg);
    static Formatter logWarning(std::string msg);

    void setSource(std::string source);

private:
    std::shared_ptr<spdlog::logger> console;
    std::string source;
    void initConsole(std::string name);
    static const int NAME_PADDING = 5;

};


#endif //PROY_GRADO_LOGGER_H
