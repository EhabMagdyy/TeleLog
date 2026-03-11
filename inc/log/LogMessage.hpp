#pragma once
#include <string>
#include <chrono>
#include "enums.hpp"

class LogMessage{
public:
    std::string appName;
    std::string context;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    std::string message;
    SeverityLvl_enum severity;

    explicit LogMessage(const std::string& app, const std::string& cntxt, const std::string& msg, SeverityLvl_enum sev, std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());
    ~LogMessage() = default;
    LogMessage(const LogMessage&) = default;
    LogMessage(LogMessage&&) noexcept = default;
    LogMessage& operator=(const LogMessage&) = default;
    LogMessage& operator=(LogMessage&&) noexcept = default;

    std::string getLogTypeString(SeverityLvl_enum severity) const;
};

std::ostream& operator<< (std::ostream& outStream, const LogMessage& log);