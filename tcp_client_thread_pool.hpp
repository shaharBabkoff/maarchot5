

#ifndef TCP_CLIENT_THREAD_POOL_H
#define TCP_CLIENT_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "pollserver.hpp"

class TcpClientThreadPool {
public:
    TcpClientThreadPool(size_t numThreads);
    ~TcpClientThreadPool();

    void enqueue(std::shared_ptr<Context> task);

private:
    std::vector<std::thread> workers;
    std::queue<std::shared_ptr<Context>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;

    void worker();
};

#endif
