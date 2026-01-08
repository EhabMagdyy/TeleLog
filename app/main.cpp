#include <iostream>
#include "ConsoleSinkImpl.hpp"
#include "LogMessage.hpp"
#include "FileSinkImpl.hpp"
#include "LogManager.hpp"
#include "ITelemetrySource.hpp"
#include "FileTelemetrySourceImpl.hpp"

int main(){
    std::cout << "Log Telemetry System" << std::endl;

   FileTelemetrySourceImpl source;

    if(!source.openSource()){
        std::cerr << "Failed to open source.txt" << std::endl;
        return 1;
    }

    std::string data;
    // Read multiple times to see how it works
    while(source.readSource(data)){
        std::cout << "Read: " << data << std::endl;
    }

    std::cout << "No more data or error occurred." << std::endl;
    

    return 0;
}