#include <iostream>
#include "ConsoleSinkImpl.hpp"
#include "LogMessage.hpp"
#include "FileSinkImpl.hpp"
#include "LogManager.hpp"
#include "ITelemetrySource.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "SocketTelemetrySourceImpl.hpp"
#include "LogFormatter.hpp"
#include "policies.hpp"

int main() {
    std::cout << "Log Telemetry System" << std::endl;

    LogManager logMang;
    logMang.addSink(std::make_unique<ConsoleSink>());

    // CPU via file
    FileTelemetrySourceImpl cpuSource;
    if(cpuSource.openSource()) {
        std::string data;
        if(cpuSource.readSource(data)) {
            LogFormatter<CpuPolicy> cpuFormatter;
            auto logMsgOpt = cpuFormatter.formatDataToLogMsg(data);
            if(logMsgOpt) logMang.addLog(*logMsgOpt);
        }
    } else {
        std::cerr << "Failed to open CPU telemetry source" << std::endl;
    }

    // RAM via socket
    SocketTelemetrySourceImpl ramSource;
    if(ramSource.openSource()) {
        std::string data;
        if(ramSource.readSource(data)) {
            LogFormatter<RamPolicy> ramFormatter;
            auto logMsgOpt = ramFormatter.formatDataToLogMsg(data);
            if(logMsgOpt) logMang.addLog(*logMsgOpt);
        }
    } else {
        std::cerr << "Failed to open RAM telemetry source" << std::endl;
    }

    // Route all logs to sinks
    logMang.routeLogsForAllSinks();

    return 0;
}
