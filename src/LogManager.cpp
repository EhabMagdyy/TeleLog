#include "LogManager.hpp"
#include "ILogSink.hpp"

void LogManager::addLog(const LogMessage& log){
    logs.push_back(log);
}

void LogManager::addSink(std::unique_ptr<ILogSink> sink){
    sinks.push_back(std::move(sink));
}

void LogManager::routeLogsForAllSinks(){
    for(auto& log : logs){
        for(auto& sink: sinks){
            sink->write(log);
        }
    }
}