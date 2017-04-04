//
// Created by bruno on 02/04/17.
//

#include "Logger.h"
#include "spdlog/spdlog.h"

Logger &Logger::getInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
    spdlog::stdout_logger_mt("console");
}

void Logger::Log(std::string msg, Logger::LogType severity) {
    switch (severity){
        case LogType::CRITICAL:
            console->critical(msg);
            break;
        case LogType::ERROR:
            console->error(msg);
            break;
        case LogType::WARNING:
            console->warn(msg);
            break;
        case LogType::INFO:
            console->info(msg);
            break;
        case LogType::DEBUG:
            std::cout << msg << std::endl;
            //console->debug(msg);
            break;
    }
}

Logger::~Logger() {
    spdlog::drop_all();
}

void Logger::LogCritical(std::string msg) {
    Logger::getInstance().Log(msg, LogType::CRITICAL);
}

void Logger::LogError(std::string msg) {
    Logger::getInstance().Log(msg, LogType::ERROR);
}

void Logger::LogWarning(std::string msg) {
    Logger::getInstance().Log(msg, LogType::WARNING);
}

void Logger::LogInfo(std::string msg) {
    Logger::getInstance().Log(msg, LogType::INFO);
}

void Logger::LogDebug(std::string msg) {
    Logger::getInstance().Log(msg, LogType::DEBUG);
}
