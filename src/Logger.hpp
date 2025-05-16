//
// Created by Daniel Adámek - daniel@nullptr.cz on 9.5.25.
// Copyright (c) 2025 FM TUL. All rights reserved.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>

enum class LogLevel {
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

class Logger {
public:
    static void error(const std::string& message);
    static void warning(const std::string& message);
    static void info(const std::string& message);
    static void debug(const std::string& message);

private:
    static void log(const LogLevel& level, const std::string& message);
};



#endif //LOGGER_HPP
