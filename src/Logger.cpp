//
// Created by Daniel Adámek - daniel@nullptr.cz on 14.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#include "Logger.hpp"

#include <iostream>
#include <ostream>

void Logger::error(const std::string &message) {
    log(LogLevel::ERROR, message);
}

void Logger::warning(const std::string &message) {
    log(LogLevel::WARNING, message);
}

void Logger::info(const std::string &message) {
    log(LogLevel::INFO, message);
}

void Logger::debug(const std::string &message) {
    log(LogLevel::DEBUG, message);
}

void Logger::log(const LogLevel &level, const std::string &message) {
    std::string prefix;
    std::string colorCode;
    switch (level) {
        case LogLevel::ERROR:
            prefix = "[ERROR] ";
            colorCode = "\033[31m"; // Red
            break;
        case LogLevel::WARNING:
            prefix = "[WARNING] ";
            colorCode = "\033[33m"; // Yellow
            break;
        case LogLevel::INFO:
            prefix = "[INFO] ";
            colorCode = "\033[32m"; // Green
            break;
        case LogLevel::DEBUG:
            prefix = "[DEBUG] ";
            colorCode = "\033[34m"; // Blue
            break;
    }
    std::cout << colorCode << prefix << message << "\033[0m\n"; // Reset color
}
