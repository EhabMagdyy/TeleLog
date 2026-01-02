#include <iostream>
#include "ConsoleSinkImpl.hpp"
#include "LogMessage.hpp"
#include "FileSinkImpl.hpp"
#include "LogManager.hpp"

int main(){
    std::cout << "Log Telemetry System Initialized." << std::endl;

    ConsoleSink console = ConsoleSink();
    LogMessage log1("MyApp", "Initialization", "Application started successfully.", LogType::INFO);
    console.write(log1);

    FileSink fileSink("app.log");
    LogMessage log2("MyApp", "FileOperation", "File opened successfully.", LogType::INFO);
    fileSink.write(log2);


    return 0;
}