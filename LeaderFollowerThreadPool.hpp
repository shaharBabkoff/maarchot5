#ifndef LEADERFOLLOWERTHREADPOOL_HPP
#define LEADERFOLLOWERTHREADPOOL_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <atomic>
#include "MSTree.hpp"

// Class to encapsulate task group state
class TaskGroup
{
public:
    TaskGroup(size_t taskCount);

    // Method to wait until all tasks in the group are complete
    void waitForTaskGroup();

    // Method to notify that a task has completed
    void taskCompleted();

private:
    std::shared_ptr<std::atomic<int>> counter; // Shared counter for task group
    std::condition_variable cv;                // Condition variable for notification
    std::mutex mtx;                            // Mutex for condition variable
};

// LFTPTask class with a process() method
class LFTPTask
{
public:
    LFTPTask(MSTree data, int fd, std::shared_ptr<TaskGroup> taskGroup);

    void process();
    virtual void execute() = 0;

private:
    std::shared_ptr<TaskGroup> taskGroup; // Shared task group instance
protected:
    MSTree data_;                           // Unique task ID
    int fd_;
};

// Leader-Follower Thread Pool Singleton class
class LeaderFollowerThreadPool
{
public:
    // Method to access the singleton instance
    static LeaderFollowerThreadPool &getInstance(size_t numThreads = 4);

    // Prevent copying and assignment
    LeaderFollowerThreadPool(const LeaderFollowerThreadPool &) = delete;
    LeaderFollowerThreadPool &operator=(const LeaderFollowerThreadPool &) = delete;

    void addTaskGroup(const std::vector<std::shared_ptr<LFTPTask>> &tasks);

private:
    // Private constructor
    LeaderFollowerThreadPool(size_t numThreads);
    ~LeaderFollowerThreadPool();

    void workerThread(int threadId);

    std::vector<std::thread> threads;
    std::queue<std::shared_ptr<LFTPTask>> taskQueue;

    std::mutex mtx;
    std::condition_variable condVar;
    bool stopPool = false;
    int currentLeader = -1;
};

void executeLeaderFollowerThreadPool(MSTree data, int fd);

#endif // LEADERFOLLOWERTHREADPOOL_HPP
