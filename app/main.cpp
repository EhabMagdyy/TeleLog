#include <iostream>
#include <thread>
#include <condition_variable>
#include <chrono>
#include "ConsoleSinkImpl.hpp"
#include "LogMessage.hpp"
#include "FileSinkImpl.hpp"
#include "LogManager.hpp"
#include "ITelemetrySource.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "SocketTelemetrySourceImpl.hpp"
#include "LogFormatter.hpp"
#include "policies.hpp"
#include "LogManagerBuilder.hpp"

std::mutex mtx;
std::condition_variable cv;
bool newLog = false;

void CPU_Task(LogManager& logManager){
    static FileTelemetrySourceImpl cpuSource;
    if(!cpuSource.openSource()){
        std::cerr << "Failed to open CPU telemetry source.\n";
        return;
    }

    while(true){
        std::string data;
        if(cpuSource.readSource(data)){
            LogFormatter<CpuPolicy> cpuFormatter;
            auto logMsgOpt = cpuFormatter.formatDataToLogMsg(data);
            if(logMsgOpt){
                logManager.addLog(*logMsgOpt);
                std::lock_guard<std::mutex> lock(mtx);
                newLog = true;
                cv.notify_one();  // Notify the routing thread that a new log is available
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void RAM_Task(LogManager& logManager){
    static SocketTelemetrySourceImpl ramSource;
    if(!ramSource.openSource()){
        std::cerr << "Failed to open RAM telemetry source.\n";
        return;
    }

    while(true){
        std::string data;
        if(ramSource.readSource(data)){
            LogFormatter<RamPolicy> ramFormatter;
            auto logMsgOpt = ramFormatter.formatDataToLogMsg(data);
            if(logMsgOpt){
                logManager.addLog(*logMsgOpt);
                std::lock_guard<std::mutex> lock(mtx);
                newLog = true;
                cv.notify_one();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void Routing_Task(LogManager& logManager){
    while(true){
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, [&logManager](){ return newLog; });  // Wait until there's a new log to route
        newLog = false;  // Reset the flag after routing logs
        lck.unlock();
        logManager.routeLogsForAllSinks();
    }
}

int main(){
    std::cout << "Log Telemetry System" << std::endl;

    // Build the LogManager with sinks
    auto logMang = LogManagerBuilder()
                        .addSink(std::make_unique<ConsoleSink>())
                        .addSink(std::make_unique<FileSink>("fileSink.txt"))
                        .build();

    
    std::thread cpu(CPU_Task, std::ref(*logMang));
    std::thread ram(RAM_Task, std::ref(*logMang));
    std::thread router(Routing_Task, std::ref(*logMang));

    cpu.join();
    ram.join();
    router.join();

    return 0;
}
