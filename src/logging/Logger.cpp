//
// Created by bruno on 02/04/17.
//

#include <iostream>
#include <cstdio>
#include "Logger.h"
#include "spdlog/spdlog.h"
#include "stdio.h"
#include "boost/format.hpp"


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
            console->debug(msg);
            break;
    }

    //console->flush();
}

Logger::~Logger() {
    spdlog::drop_all();
}

Logger::Formatter Logger::logDebug(std::string msg) {
    return Formatter(msg, LogType::DEBUG);
}

Logger::Formatter Logger::logInfo(std::string msg) {
    return Formatter(msg, LogType::INFO);
}

Logger::Formatter Logger::logCritical(std::string msg) {
    return Formatter(msg, LogType::CRITICAL);
}

Logger::Formatter Logger::logError(std::string msg) {
    return Formatter(msg, LogType::ERROR);
}

Logger::Formatter Logger::logWarning(std::string msg) {
    return Formatter(msg, LogType::WARNING);
}

void Logger::setSource(std::string source) {
    this->source = source;
    initConsole(source);
}