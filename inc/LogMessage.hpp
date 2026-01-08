#pragma once
#include <string>
#include <chrono>

enum class LogType{
    INFO,
    WARNING,
    ERROR
};

class LogMessage{
public:
    std::string appName;
    std::string context;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    std::string message;
    LogType severity;

    explicit LogMessage(const std::string& app, const std::string& cntxt, const std::string& msg, LogType sev, std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());
    ~LogMessage() = default;
    LogMessage(const LogMessage&) = default;
    LogMessage(LogMessage&&) noexcept = default;
    LogMessage& operator=(const LogMessage&) = default;
    LogMessage& operator=(LogMessage&&) noexcept = default;

    std::string getLogTypeString(LogType severity) const;
};

std::ostream& operator<< (std::ostream& outStream, const LogMessage& log);