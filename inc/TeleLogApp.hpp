#pragma once

#include <atomic>
#include <csignal>
#include <condition_variable>
#include "log/LogManager.hpp"
#include "ThreadPool.hpp"

class TeleLogApp
{
private:
    std::mutex              mtx;
    std::condition_variable cv;
    bool                    newLog = false;
    std::atomic<bool>       running = true;
    std::shared_ptr<LogManager> logManager;
    std::unique_ptr<ThreadPool> pool;
    static TeleLogApp* instance_;
    static void staticSignalHandler(int sig);

    void submitLog(LogManager& logManager, std::optional<LogMessage> logMsgOpt);
    void CPU_Task(LogManager& logManager);
    void RAM_Task(LogManager& logManager);
    void GPU_Task(LogManager& logManager);
    void Routing_Task(LogManager& logManager);
    void signalHandler(int sig);

public:
    TeleLogApp();
    ~TeleLogApp();

    void start();
};