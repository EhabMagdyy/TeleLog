#include "ThreadPool.hpp"

// mutex lock look in a scoope to release it after the desired operation is done

// Initializes the thread pool and starts worker threads
ThreadPool::ThreadPool(size_t numThreads) : stop(false){
    for(size_t i = 0; i < numThreads; ++i){
        workers.emplace_back([this]{
            while(true){
                std::function<void()> task;
                {
                    // Lock the task queue and wait for a pushed/enqueued task or stop signal
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this]{
                        return stop || !tasks.empty();
                    });
                    // Received a stop signal and no tasks left, exit the thread
                    if(stop && tasks.empty()){
                        return;
                    }
                    // There is still a task to process, pop it from the queue
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task(); // execute outside the lock
            }
        });
    }
}

ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all(); // wake all sleeping threads
    for(auto& worker : workers){
        worker.join();      // wait for each to finish
    }
}

void ThreadPool::push(std::function<void()> task){
    {
        std::unique_lock<std::mutex> lock(queueMutex);  // lock the queue to add a new task
        tasks.emplace(std::move(task));     // add the task to the queue
    }
    condition.notify_one(); // wake one sleeping thread(There is a task to work on, ya slave)
}