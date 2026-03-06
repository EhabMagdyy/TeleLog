#include <iostream>
#include <atomic>
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
#include "ThreadPool.hpp"

std::mutex mtx;
std::condition_variable cv;
bool newLog  = false;
std::atomic<bool> running = true;   // shared stop signal for all tasks

void CPU_Task(LogManager& logManager){
    static FileTelemetrySourceImpl cpuSource;
    if(!cpuSource.openSource()){
        std::cerr << "Failed to open CPU telemetry source.\n";
        return;
    }

    while(running){
        std::string data;
        if(cpuSource.readSource(data)){
            LogFormatter<CpuPolicy> cpuFormatter;
            auto logMsgOpt = cpuFormatter.formatDataToLogMsg(data);
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

void RAM_Task(LogManager& logManager){
    static SocketTelemetrySourceImpl ramSource;
    if(!ramSource.openSource()){
        std::cerr << "Failed to open RAM telemetry source.\n";
        return;
    }

    while(running){
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
    while(running){
        std::unique_lock<std::mutex> lck(mtx);
        // wait_for so it wakes periodically to re-check `running`
        cv.wait_for(lck, std::chrono::milliseconds(500),
                    []{ return newLog; });
        if(newLog){
            newLog = false;
            lck.unlock();
            logManager.routeLogsForAllSinks();
        }
    }
}

int main(){
    std::cout << "Log Telemetry System" << std::endl;

    auto logMang = LogManagerBuilder()
                        .addSink(std::make_unique<ConsoleSink>())
                        .addSink(std::make_unique<FileSink>("fileSink.txt"))
                        .build();

    ThreadPool pool(3);  // 3 workers

    // Enqueue the 3 tasks, each will loop until "running" is false
    // lambda function to Captures the logManag to pass it by reference to each task
    pool.push([&](){ CPU_Task(*logMang); });
    pool.push([&](){ RAM_Task(*logMang); });
    pool.push([&](){ Routing_Task(*logMang); });

    return 0;
}