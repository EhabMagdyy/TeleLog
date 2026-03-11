#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataProxy.hpp>
#include <iostream>
#include <atomic>
#include <csignal>
#include <condition_variable>
#include <chrono>
#include <iomanip>
#include "sink/ConsoleSinkImpl.hpp"
#include "sink/FileSinkImpl.hpp"
#include "log/LogManager.hpp"
#include "log/LogMessage.hpp"
#include "log/LogFormatter.hpp"
#include "log/LogManagerBuilder.hpp"
#include "source/FileTelemetrySourceImpl.hpp"
#include "source/SocketTelemetrySourceImpl.hpp"
#include "source/SomeIPTelemetrySourceImpl.hpp"
#include "policies.hpp"
#include "ThreadPool.hpp"

std::mutex              mtx;
std::condition_variable cv;
bool                    newLog = false;
std::atomic<bool>       running = true;

void signalHandler(int) {
    running = false;
}

// helpers shared by all tasks
void submitLog(LogManager& logManager, std::optional<LogMessage> logMsgOpt) {
    if (logMsgOpt) {
        logManager.addLog(*logMsgOpt);
        std::lock_guard<std::mutex> lock(mtx);
        newLog = true;
        cv.notify_one();
    }
}

// tasks
void CPU_Task(LogManager& logManager) {
    static FileTelemetrySourceImpl cpuSource;
    if (!cpuSource.openSource()) {
        std::cerr << "Failed to open CPU telemetry source.\n";
        return;
    }
    while (running) {
        std::string data;
        if (cpuSource.readSource(data))
            submitLog(logManager, LogFormatter<CpuPolicy>::formatDataToLogMsg(data));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void RAM_Task(LogManager& logManager) {
    static SocketTelemetrySourceImpl ramSource;
    if (!ramSource.openSource()) {
        std::cerr << "Failed to open RAM telemetry source.\n";
        return;
    }
    while (running) {
        std::string data;
        if (ramSource.readSource(data))
            submitLog(logManager, LogFormatter<RamPolicy>::formatDataToLogMsg(data));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void GPU_Task(LogManager& logManager) {
    static SomeIPTelemetrySourceImpl gpuSource;
    if (!gpuSource.openSource()) {
        std::cerr << "Failed to open GPU telemetry source.\n";
        return;
    }

    while (running) {
        std::string data;
        if (gpuSource.readSource(data))
            submitLog(logManager, LogFormatter<GpuPolicy>::formatDataToLogMsg(data));
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}

void Routing_Task(LogManager& logManager) {
    while (running) {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait_for(lck, std::chrono::milliseconds(500), [] { return newLog; });
        if (newLog) {
            newLog = false;
            lck.unlock();
            logManager.routeLogsForAllSinks();
        }
    }
}

// main
int main() {
    std::signal(SIGINT, signalHandler);
    std::cout << "Log Telemetry System\n";

    auto logMang = LogManagerBuilder()
                       .addSink(std::make_unique<ConsoleSink>())
                       .addSink(std::make_unique<FileSink>("fileSink.txt"))
                       .build();

    ThreadPool pool(4);   // 4 workers: CPU, RAM, GPU, Routing

    pool.push([&]() { CPU_Task(*logMang);     });
    pool.push([&]() { RAM_Task(*logMang);     });
    pool.push([&]() { GPU_Task(*logMang);     });
    pool.push([&]() { Routing_Task(*logMang); });

    while (running)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "\nShutting down...\n";
    cv.notify_all();

    return 0;
}