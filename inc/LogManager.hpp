#pragma once
#include "LogMessage.hpp"
#include "ILogSink.hpp"
#include <vector>
#include <memory>

class LogManager{
    std::vector<LogMessage> logs;
    std::vector<std::unique_ptr<ILogSink>> sinks;
public:
    LogManager() = default;
    ~LogManager() = default;
    void addLog(const LogMessage& log);
    void addSink(std::unique_ptr<ILogSink> sink);
    void routeLogsForAllSinks();
};
