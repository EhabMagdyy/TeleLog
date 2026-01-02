#include <iostream>
#include "ConsoleSinkImpl.hpp"
#include "LogMessage.hpp"
#include "FileSinkImpl.hpp"
#include "LogManager.hpp"

int main(){
    std::cout << "Log Telemetry System" << std::endl;

    LogManager logMang;

    logMang.addSink(std::make_unique<ConsoleSink>());
    logMang.addSink(std::make_unique<FileSink>("app.log"));

    logMang.addLog(LogMessage("InfoApp", "Out of Cotext", "Algeria is the strongest team in AFCON 2025.", LogType::INFO));
    logMang.addLog(LogMessage("WarnApp", "Reminder", "Inter will take the title.", LogType::WARNING));

    logMang.routeLogsForAllSinks();

    FileSink fs("err.log");
    fs.write(LogMessage("ErrorApp", "Context bla bla", "Cristiano Ronaldo is the GOAT.", LogType::ERROR));
    

    return 0;
}