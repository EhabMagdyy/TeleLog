#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataProxy.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "sink/ConsoleSinkImpl.hpp"
#include "sink/FileSinkImpl.hpp"
#include "log/LogMessage.hpp"
#include "log/LogFormatter.hpp"
#include "log/LogManagerBuilder.hpp"
#include "source/FileTelemetrySourceImpl.hpp"
#include "source/SocketTelemetrySourceImpl.hpp"
#include "source/SomeIPTelemetrySourceImpl.hpp"
#include "policies.hpp"
#include "TeleLogApp.hpp"
#include "json.hpp"

using json = nlohmann::json;

TeleLogApp* TeleLogApp::instance_ = nullptr;

TeleLogApp::TeleLogApp() {
    instance_ = this;
    std::signal(SIGINT, staticSignalHandler);

    std::cout << "Log Telemetry System\n";

    logManager = LogManagerBuilder()
                       .addSink(std::make_unique<ConsoleSink>())
                       .addSink(std::make_unique<FileSink>("fileSink.txt"))
                       .build();

    pool = std::make_unique<ThreadPool>(4);   // 4 workers: CPU, RAM, GPU, Routing

    pool->push([&]() { CPU_Task(*logManager);     });
    pool->push([&]() { RAM_Task(*logManager);     });
    pool->push([&]() { GPU_Task(*logManager);     });
    pool->push([&]() { Routing_Task(*logManager); });
}

TeleLogApp::~TeleLogApp() {
    // Destructor can be used for cleanup if needed
}

void TeleLogApp::start() {
    while(running)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "\nShutting down...\n";
    cv.notify_all();
}


void TeleLogApp::staticSignalHandler(int sig) {
    if (instance_) 
        instance_->signalHandler(sig);
}

void TeleLogApp::signalHandler(int) {
    running = false;
}

// helpers shared by all tasks
void TeleLogApp::submitLog(LogManager& logManager, std::optional<LogMessage> logMsgOpt) {
    if(logMsgOpt) {
        logManager.addLog(*logMsgOpt);
        std::lock_guard<std::mutex> lock(mtx);
        newLog = true;
        cv.notify_one();
    }
}

// tasks
void TeleLogApp::CPU_Task(LogManager& logManager) {
    static FileTelemetrySourceImpl cpuSource;
    if(!cpuSource.openSource()) {
        std::cerr << "Failed to open CPU telemetry source.\n";
        return;
    }
    while(running) {
        std::string data;
        if(cpuSource.readSource(data))
            submitLog(logManager, LogFormatter<CpuPolicy>::formatDataToLogMsg(data));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void TeleLogApp::RAM_Task(LogManager& logManager) {
    static SocketTelemetrySourceImpl ramSource;
    if(!ramSource.openSource()) {
        std::cerr << "Failed to open RAM telemetry source.\n";
        return;
    }
    while(running) {
        std::string data;
        if(ramSource.readSource(data))
            submitLog(logManager, LogFormatter<RamPolicy>::formatDataToLogMsg(data));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void TeleLogApp::GPU_Task(LogManager& logManager) {
    // Create the singleton instance with a lambda handler that submits logs to the log manager on every incoming event
    auto& gpuSource = SomeIPTelemetrySourceImpl::getInstance(
        [&](std::string raw) {
            submitLog(logManager, LogFormatter<GpuPolicy>::formatDataToLogMsg(raw));
        }
    );

    if(!gpuSource.openSource()) {
        std::cerr << "Failed to open GPU telemetry source.\n";
        return;
    }

    while(running) {
        std::string data;
        if(gpuSource.readSource(data))
            submitLog(logManager, LogFormatter<GpuPolicy>::formatDataToLogMsg(data));
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}

void TeleLogApp::Routing_Task(LogManager& logManager) {
    while(running) {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait_for(lck, std::chrono::milliseconds(500), [this] { return newLog; });
        if(newLog) {
            newLog = false;
            lck.unlock();
            logManager.routeLogsForAllSinks();
        }
    }
}