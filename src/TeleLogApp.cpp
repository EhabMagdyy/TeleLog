#include "TeleLogApp.hpp"
#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataProxy.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "sink/ConsoleSinkImpl.hpp"
#include "sink/FileSinkImpl.hpp"
#include "log/LogFormatter.hpp"
#include "log/LogManagerBuilder.hpp"
#include "source/FileTelemetrySourceImpl.hpp"
#include "source/SocketTelemetrySourceImpl.hpp"
#include "source/SomeIPTelemetrySourceImpl.hpp"
#include "policies.hpp"

// =========================================== Signal Handler ==========================================
TeleLogApp* TeleLogApp::appInstance = nullptr;

void TeleLogApp::staticSignalHandler(int sig){
    if(appInstance) 
        appInstance->signalHandler(sig);
}

void TeleLogApp::signalHandler(int){
    running = false;
    cv.notify_all();
}

// ============= Loading config from passed json file or pass default configs if not found =============
json TeleLogApp::loadConfig(const std::string& path){
    json config;
    try{
        std::ifstream file(path);
        if(!file.is_open())
            throw std::runtime_error("config.json not found");
        config = json::parse(file);
    }
    catch(const std::exception& e){
        std::cerr << "[Config] " << e.what() << " — using defaults.\n";
        // Hardcoded defaults — app runs even without a config file
        config ={
           {"app_name", APP_NAME},
            {"sources",{
               {"cpu",{{"enabled", CPU_ENABLED}, {"rate_ms", CPU_RATE}}},
               {"ram",{{"enabled", RAM_ENABLED}, {"rate_ms", RAM_RATE}}},
               {"gpu",{{"enabled", GPU_ENABLED}, {"rate_ms", GPU_RATE}}}
            }},
           {"sinks",{
               {"console",   CONSOLE_SINK_ENABLED},
               {"file",      FILE_SINK_ENABLED},
               {"file_path", "fileSink.txt"}
            }},
           {"routing_rate_ms", ROUTING_RATE}
        };
    }
    return config;
}

// ================== Applying configs on startup & when the config file updated =====================
void TeleLogApp::applyConfig(const json& config){
    // Rates
    cpuRate.store(config["sources"]["cpu"].value("rate_ms", CPU_RATE));
    ramRate.store(config["sources"]["ram"].value("rate_ms", RAM_RATE));
    gpuRate.store(config["sources"]["gpu"].value("rate_ms", GPU_RATE));
    routingRate.store(config.value("routing_rate_ms", ROUTING_RATE));

    // Source enable flags
    cpuEnabled.store(config["sources"]["cpu"].value("enabled", CPU_ENABLED));
    ramEnabled.store(config["sources"]["ram"].value("enabled", RAM_ENABLED));
    gpuEnabled.store(config["sources"]["gpu"].value("enabled", GPU_ENABLED));

    // Sinks — only rebuild if something changed
    bool newConsole = config["sinks"].value("console", CONSOLE_SINK_ENABLED);
    bool newFile = config["sinks"].value("file",    FILE_SINK_ENABLED);

    if(newConsole != consoleSinkEnabled.load() || newFile != fileSinkEnabled.load()){
        consoleSinkEnabled.store(newConsole);
        fileSinkEnabled.store(newFile);
        rebuildSinks(config);
    }

    std::cout << "[Config] Applied - "
              << "CPU:" <<(cpuEnabled ? "on" : "off") << "@" << cpuRate << "ms  "
              << "RAM:" <<(ramEnabled ? "on" : "off") << "@" << ramRate << "ms  "
              << "GPU:" <<(gpuEnabled ? "on" : "off") << "@" << gpuRate << "ms  "
              << "Routing@" << routingRate << "ms\n";
}

// ================================ Rebuild sinks when config changes ===============================
void TeleLogApp::rebuildSinks(const json& config){
    std::lock_guard<std::mutex> lock(mtx);

    auto builder = LogManagerBuilder();
    if(consoleSinkEnabled.load())
        builder.addSink(std::make_unique<ConsoleSink>());
    if(fileSinkEnabled.load())
        builder.addSink(std::make_unique<FileSink>(
            config["sinks"].value("file_path", "fileSink.txt")));

    logManager = builder.build();
    std::cout << "[Config] Sinks rebuilt - "
              << "console:" <<(consoleSinkEnabled ? "on   " : "off   ")
              << "file:" <<(fileSinkEnabled ? "on\n" : "off\n");
}

// ======================================== Constructor =========================================== 
TeleLogApp::TeleLogApp(const std::string& configPath) : configPath(configPath){
    // Signal handler
    appInstance = this;
    std::signal(SIGINT, staticSignalHandler);

    // Load and apply config
    json config = loadConfig(configPath);

    std::cout << config.value("app_name", "[TeleLog] Log Telemetry System");

    // Build initial sinks
    consoleSinkEnabled.store(config["sinks"].value("console", true));
    fileSinkEnabled.store  (config["sinks"].value("file",    true));

    auto builder = LogManagerBuilder();
    if(consoleSinkEnabled.load())
        builder.addSink(std::make_unique<ConsoleSink>());
    if(fileSinkEnabled.load())
        builder.addSink(std::make_unique<FileSink>(
            config["sinks"].value("file_path", "fileSink.txt")));
    logManager = builder.build();

    // Apply rates and flags
    applyConfig(config);

    // Launch tasks — 5 workers: CPU, RAM, GPU, Routing, ConfigWatcher
    pool = std::make_unique<ThreadPool>(5);
    pool->push([this](){ CPU_Task();});
    pool->push([this](){ RAM_Task();});
    pool->push([this](){ GPU_Task();});
    pool->push([this](){ Routing_Task();});
    pool->push([this](){ ConfigWatcher_Task();});
}

// ====================================== Start the application =========================================
void TeleLogApp::start(){
    while(running)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "\nShutting down...\n";
    cv.notify_all();
}

// ======================== Add new log to log manager & notify the routing task ========================
void TeleLogApp::submitLog(LogManager& logManager, std::optional<LogMessage> msg){
    if(msg){
        logManager.addLog(*msg);
        std::lock_guard<std::mutex> lock(mtx);
        newLog.store(true);
        cv.notify_one();
    }
}

// =========================================== System Tasks =============================================
void TeleLogApp::CPU_Task(){
    static FileTelemetrySourceImpl cpuSource;
    if(!cpuSource.openSource()){
        std::cerr << "Failed to open CPU telemetry source.\n";
        return;
    }
    while(running){
        if(cpuEnabled.load()){
            std::string data;
            auto lm = logManager;
            if(cpuSource.readSource(data))
                submitLog(*lm, LogFormatter<CpuPolicy>::formatDataToLogMsg(data));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(cpuRate.load()));
    }
}

void TeleLogApp::RAM_Task(){
    static SocketTelemetrySourceImpl ramSource;
    if(!ramSource.openSource()){
        std::cerr << "Failed to open RAM telemetry source.\n";
        return;
    }
    while(running){
        if(ramEnabled.load()){
            std::string data;
            auto lm = logManager;
            if(ramSource.readSource(data))
                submitLog(*lm, LogFormatter<RamPolicy>::formatDataToLogMsg(data));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ramRate.load()));
    }
}

void TeleLogApp::GPU_Task(){
    // Create gpu telemetry source & add callback handler for receiving interrupts
    auto lm = logManager;
    auto& gpuSource = SomeIPTelemetrySourceImpl::getInstance(
        [this, lm](std::string raw){
            // fires whenever server broadcasts
            if(gpuEnabled.load())
                submitLog(*lm, LogFormatter<GpuPolicy>::formatDataToLogMsg(raw));
        }
    );

    if(!gpuSource.openSource()){
        std::cerr << "Failed to open GPU telemetry source.\n";
        return;
    }
    // Request GPU Readings
    while(running){
        if(gpuEnabled.load()){
            std::string data;
            if(gpuSource.readSource(data))
                submitLog(*lm, LogFormatter<GpuPolicy>::formatDataToLogMsg(data));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(gpuRate.load()));
    }
}

void TeleLogApp::Routing_Task(){
    while(running){
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait_for(lck, std::chrono::milliseconds(routingRate.load()),
            [this]{ return newLog.load() || !running; });
        if(newLog.exchange(false)){
            auto lm = logManager;
            lck.unlock();
            lm->routeLogsForAllSinks();
        }
    }
}

// Checks if config.json changed to modify json config & apply new configs
void TeleLogApp::ConfigWatcher_Task(){
    namespace fs = std::filesystem;
    std::error_code errorCode;
    auto lastStableWrite = fs::last_write_time(configPath, errorCode);

    while(running){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        auto currentWrite = fs::last_write_time(configPath, errorCode);
        if (errorCode || currentWrite == lastStableWrite)
            continue;

        // File changed -> wait until write time stops moving
        auto previousWrite = currentWrite;
        while(running){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            currentWrite = fs::last_write_time(configPath, errorCode);

            if(currentWrite == previousWrite)
                break;                     // Stable now -> safe to read
            previousWrite = currentWrite;  // still changing -> keep waiting
        }
        // Now safe to read
        lastStableWrite = currentWrite;
        json newConfig = loadConfig(configPath);
        applyConfig(newConfig);
    }
}