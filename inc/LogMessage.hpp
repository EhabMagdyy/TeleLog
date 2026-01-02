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

    LogMessage(const std::string& app, const std::string& cntxt, const std::string& msg, LogType sev);
    ~LogMessage() = default;

    std::string getLogTypeString(LogType& severity) const;
};

std::ostream& operator<< (std::ostream& outStream, const LogMessage& log);