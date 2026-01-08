#include <iostream>
#include "ConsoleSinkImpl.hpp"
#include "LogMessage.hpp"
#include "FileSinkImpl.hpp"
#include "LogManager.hpp"
#include "ITelemetrySource.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "SocketTelemetrySourceImpl.hpp"

int main(){
    std::cout << "Log Telemetry System" << std::endl;

    LogManager logMang;
    logMang.addSink(std::make_unique<ConsoleSink>());
    
    FileTelemetrySourceImpl source;
    if(!source.openSource()){
        std::cerr << "Failed to open source.txt" << std::endl;
        return 1;
    }
    std::string fileData;
    if(source.readSource(fileData)){
        fileData.push_back('%');
        LogType type = LogType::INFO;
        double cpuUsage = 0.0;
        try {
            cpuUsage = std::stod(fileData);
        } catch(...) { cpuUsage = 0.0; }
        if(cpuUsage > 75.0) type = LogType::WARNING;

        logMang.addLog(LogMessage("SysMonitor", "CPU Usage", fileData, type));
    }    

    SocketTelemetrySourceImpl socketSource;
    if(!socketSource.openSource()){
        std::cerr << "Failed to open socket source" << std::endl;
        return 1;
    }
    std::string socketData;
    if(socketSource.readSource(socketData)){
        socketData.push_back('%');
        LogType type = LogType::INFO;
        double cpuUsage = 0.0;
        try {
            cpuUsage = std::stod(socketData);
        } catch(...) { cpuUsage = 0.0; }
        if(cpuUsage > 75.0) type = LogType::WARNING;

        logMang.addLog(LogMessage("SysMonitor", "Memory Usage", socketData, type));
    }

    logMang.routeLogsForAllSinks();

    return 0;
}