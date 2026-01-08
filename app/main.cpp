#include <iostream>
#include "ConsoleSinkImpl.hpp"
#include "LogMessage.hpp"
#include "FileSinkImpl.hpp"
#include "LogManager.hpp"
#include "ITelemetrySource.hpp"
#include "FileTelemetrySourceImpl.hpp"

int main(){
    std::cout << "Log Telemetry System" << std::endl;

    // Log & Sink Testing
    LogManager logMang;
    logMang.addSink(std::make_unique<ConsoleSink>());
    logMang.addSink(std::make_unique<FileSink>("app.log"));

    logMang.addLog(LogMessage("InfoApp", "Out of Cotext", "Algeria is the strongest team in AFCON 2025.", LogType::INFO));
    logMang.addLog(LogMessage("WarnApp", "Reminder", "Inter will take the title.", LogType::WARNING));

    logMang.routeLogsForAllSinks();
    FileSink fs("err.log");
    fs.write(LogMessage("ErrorApp", "Context bla bla", "Cristiano Ronaldo is the GOAT.", LogType::ERROR));

    // File Source Testing
    FileTelemetrySourceImpl source;
    if(!source.openSource()){
        std::cerr << "Failed to open source.txt" << std::endl;
        return 1;
    }
    std::string data;
    while(source.readSource(data)){
        std::cout << "Read: " << data << std::endl;
    }    

    return 0;
}