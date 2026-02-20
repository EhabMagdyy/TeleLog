#include "LogManager.hpp"
#include "ILogSink.hpp"

void LogManager::addLog(const LogMessage& log){
    logs.tryPush(std::move(log));
}

void LogManager::addSink(std::unique_ptr<ILogSink> sink){
    sinks.push_back(std::move(sink));
}

void LogManager::routeLogsForAllSinks(){
    while (auto log = logs.tryPop()) {
    for (auto& sink : sinks) {
        sink->write(*log);
    }
}
}