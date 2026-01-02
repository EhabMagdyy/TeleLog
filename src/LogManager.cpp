#include "LogManager.hpp"
#include "ILogSink.hpp"

void LogManager::addLog(const LogMessage& log){
    logs.push_back(log);
}

void LogManager::addSink(const ILogSink& sink){
    sinks.push_back(sink);
}

void LogManager::routeLogsForAllSinks(){
    for(auto& log : logs){
        for(auto& sink: sinks){
            sink.write(log);
        }
    }
}