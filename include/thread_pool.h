//
// Created by prbou on 1/13/2025.
//

#ifndef OPENGL_FUN_THREAD_POOL_H
#define OPENGL_FUN_THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

//class ThreadPool {
//public:
//    ThreadPool(size_t numThreads) {
//        for (size_t i = 0; i < numThreads; ++i) {
//            workers.emplace_back([this]() {
//                while (true) {
//                    std::function<void()> task;
//                    {
//                        std::unique_lock<std::mutex> lock(queueMutex);
//                        condition.wait(lock, [this]() { return stop || !tasks.empty(); });
//                        if (stop && tasks.empty()) return;
//                        task = std::move(tasks.front());
//                        tasks.pop();
//                    }
//                    task();  // Execute the task
//                }
//            });
//        }
//    }
//
//    template<class F>
//    void enqueue(F&& f) {
//        {
//            std::unique_lock<std::mutex> lock(queueMutex);
//            tasks.emplace(std::forward<F>(f));
//        }
//        condition.notify_one();
//    }
//
//    ~ThreadPool() {
//        {
//            std::unique_lock<std::mutex> lock(queueMutex);
//            stop = true;
//        }
//        condition.notify_all();
//        for (std::thread& worker : workers) {
//            worker.join();
//        }
//    }
//
//private:
//    std::vector<std::thread> workers;
//    std::queue<std::function<void()>> tasks;
//    std::mutex queueMutex;
//    std::condition_variable condition;
//    bool stop = false;
//};

class ThreadPool {
public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();  // Execute the task
                    {
                        std::unique_lock<std::mutex> lock(completionMutex);
                        if (--activeTaskCount == 0) {
                            completionCondition.notify_all();
                        }
                    }
                }
            });
        }
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            ++activeTaskCount;  // Increment before enqueueing
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    void wait_completion() {
        std::unique_lock<std::mutex> lock(completionMutex);
        completionCondition.wait(lock, [this]() { return activeTaskCount == 0; });
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;

    // Task completion tracking
    std::atomic<size_t> activeTaskCount{0};
    std::mutex completionMutex;
    std::condition_variable completionCondition;
};


#endif //OPENGL_FUN_THREAD_POOL_H
