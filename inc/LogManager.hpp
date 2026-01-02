#pragma once
#include "LogMessage.hpp"
#include "ILogSink.hpp"
#include <vector>

class LogManager{
    std::vector<LogMessage> logs;
    std::vector<ILogSink> sinks;
public:
    LogManager() = default;
    ~LogManager() = default;
    void addLog(const LogMessage& log);
    void addSink(const ILogSink& sink);
    void routeLogsForAllSinks();
};
