#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    void push(std::function<void()> task);

private:
    std::vector<std::thread>          workers;  // worker threads
    std::queue<std::function<void()>> tasks;    // task queue
    std::mutex                        queueMutex; // mutex for task queue
    std::condition_variable           condition;  // condition variable for task notification
    bool                              stop;       // flag to stop the pool
};