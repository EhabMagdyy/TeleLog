#pragma once
#include <atomic>
#include <csignal>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include "log/LogManager.hpp"
#include "ThreadPool.hpp"
#include "json.hpp"

using json = nlohmann::json;

// Default Configs
#define APP_NAME                "[TeleLog] Log Telemetry System"
#define CPU_RATE                1000
#define RAM_RATE                1000
#define GPU_RATE                1000
#define ROUTING_RATE            100
#define CPU_ENABLED             true
#define RAM_ENABLED             true
#define GPU_ENABLED             true
#define CONSOLE_SINK_ENABLED    true
#define FILE_SINK_ENABLED       false

class TeleLogApp {
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> newLog {false};
    std::atomic<bool> running {true};

    std::shared_ptr<LogManager> logManager;
    std::unique_ptr<ThreadPool> pool;
    std::string configPath;

    std::string appName {APP_NAME};

    std::atomic<int> cpuRate {CPU_RATE};
    std::atomic<int> ramRate {RAM_RATE};
    std::atomic<int> gpuRate {GPU_RATE};
    std::atomic<int> routingRate {ROUTING_RATE};

    std::atomic<bool> cpuEnabled {CPU_ENABLED};
    std::atomic<bool> ramEnabled {RAM_ENABLED};
    std::atomic<bool> gpuEnabled {GPU_ENABLED};

    std::atomic<bool> consoleSinkEnabled {CONSOLE_SINK_ENABLED};
    std::atomic<bool> fileSinkEnabled {FILE_SINK_ENABLED};

    static TeleLogApp* appInstance;
    static void staticSignalHandler(int sig);
    void signalHandler(int sig);

    json loadConfig(const std::string& path);
    void applyConfig(const json& config);
    void rebuildSinks(const json& config);

    void submitLog(LogManager& logManager, std::optional<LogMessage> msg);

    void CPU_Task();
    void RAM_Task();
    void GPU_Task();
    void Routing_Task();
    void ConfigWatcher_Task();

public:
    explicit TeleLogApp(const std::string& configPath = "config.json");
    ~TeleLogApp() = default;

    TeleLogApp(const TeleLogApp&)            = delete;
    TeleLogApp& operator=(const TeleLogApp&) = delete;
    TeleLogApp(TeleLogApp&&)                 = delete;
    TeleLogApp& operator=(TeleLogApp&&)      = delete;

    void start();
};