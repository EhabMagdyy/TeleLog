#pragma once
#include "LogMessage.hpp"
#include "ILogSink.hpp"
#include "RingBuffer.hpp"
#include <vector>
#include <memory>

class LogManager{
    RingBuffer<LogMessage> logs{20};  // will hold the most recent 20 logs
    std::vector<std::unique_ptr<ILogSink>> sinks;
public:
    LogManager() = default;
    ~LogManager() = default;
    void addLog(const LogMessage& log);
    void addSink(std::unique_ptr<ILogSink> sink);
    void routeLogsForAllSinks();
};
