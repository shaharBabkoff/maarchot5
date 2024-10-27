

#ifndef TCP_CLIENT_THREAD_POOL_H
#define TCP_CLIENT_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "pollserver.hpp"
// Thread pool for handling client requests in parallel
class TcpClientThreadPool
{
public:
    TcpClientThreadPool(size_t numThreads);
    ~TcpClientThreadPool();
    // Adds a new client task (Context) to the task queue
    void enqueue(std::shared_ptr<Context> task);

private:
    std::vector<std::thread> workers;           // Threads in the pool
    std::queue<std::shared_ptr<Context>> tasks; // Queue to hold pending tasks
    std::mutex queueMutex;             // Mutex to protect access to the task queue
    std::condition_variable condition; // Condition variable to notify worker threads of new tasks
    bool stop;                         // Flag to signal threads to stop processing

    // Main function executed by each worker thread
    void worker();
};

#endif
