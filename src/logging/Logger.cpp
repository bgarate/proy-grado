//
// Created by bruno on 02/04/17.
//

#include <iostream>
#include "Logger.h"
#include "spdlog/spdlog.h"
#include "stdio.h"


Logger &Logger::getInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
}

void Logger::initConsole(std::string name) {

    std::vector<spdlog::sink_ptr> sinks;
    auto stdout_sink = spdlog::sinks::stdout_sink_mt::instance();
    auto color_sink = std::make_shared<spdlog::sinks::ansicolor_sink>(stdout_sink);
    sinks.push_back(color_sink);

    console = std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
    spdlog::register_logger(console);

    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S] [%n] [%l] %v");
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

    //console->flush();
}

Logger::~Logger() {
    spdlog::drop_all();
}

void Logger::logCritical(std::string msg) {
    Logger::getInstance().Log(msg, LogType::CRITICAL);
}

void Logger::logError(std::string msg) {
    Logger::getInstance().Log(msg, LogType::ERROR);
}

void Logger::logWarning(std::string msg) {
    Logger::getInstance().Log(msg, LogType::WARNING);
}

void Logger::logInfo(std::string msg) {
    Logger::getInstance().Log(msg, LogType::INFO);
}

void Logger::logDebug(std::string msg) {
    Logger::getInstance().Log(msg, LogType::DEBUG);
}

void Logger::setSource(std::string source) {
    this->source = source;
    initConsole(source);
}
