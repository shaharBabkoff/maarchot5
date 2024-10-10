#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sstream>
#include "LeaderFollowerThreadPool.hpp"

// TaskGroup class implementation
TaskGroup::TaskGroup(size_t taskCount)
    : counter(std::make_shared<std::atomic<int>>(taskCount)) {}

void TaskGroup::waitForTaskGroup()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]
            { return *counter == 0; });
}

void TaskGroup::taskCompleted()
{
    if (--(*counter) == 0)
    {
        std::lock_guard<std::mutex> lock(mtx);
        cv.notify_one();
    }
}

// LFTPTask class implementation
LFTPTask::LFTPTask(MSTree data, int fd, std::shared_ptr<TaskGroup> taskGroup)
    : taskGroup(taskGroup), data_(data), fd_(fd) {}

void LFTPTask::process()
{
    execute();
    taskGroup->taskCompleted(); // Notify task completion
}

// Leader-Follower Thread Pool singleton implementation
LeaderFollowerThreadPool &LeaderFollowerThreadPool::getInstance(size_t numThreads)
{
    static LeaderFollowerThreadPool instance(numThreads);
    return instance;
}

LeaderFollowerThreadPool::LeaderFollowerThreadPool(size_t numThreads)
{
    // Create worker threads
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(&LeaderFollowerThreadPool::workerThread, this);
    }
}

LeaderFollowerThreadPool::~LeaderFollowerThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        stopPool = true;
    }
    condVar.notify_all();
    for (auto &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

std::shared_ptr<TaskGroup> LeaderFollowerThreadPool::addTaskGroup(const std::vector<std::shared_ptr<LFTPTask>> &tasks)
{
    // Create a task group with a counter set to the size of the tasks vector
    std::shared_ptr<TaskGroup> taskGroup = std::make_shared<TaskGroup>(tasks.size());

    std::unique_lock<std::mutex> lock(mtx);
    for (const auto &task : tasks)
    {
        taskQueue.push(task);
    }
    condVar.notify_all(); // Notify threads that new tasks are available
    return taskGroup;
}

void LeaderFollowerThreadPool::workerThread()
{
    while (true)
    {
        std::shared_ptr<LFTPTask> task;

        {
            std::unique_lock<std::mutex> lock(mtx);

            // Wait until there are tasks available or the pool is stopped
            condVar.wait(lock, [this]
                         { return !taskQueue.empty() || stopPool; });

            if (stopPool || taskQueue.empty())
            {
                break;
            }

            // Pick the next task
            task = taskQueue.front();
            taskQueue.pop();
        }

        // Process the task outside the critical section
        if (task)
        {
            task->process();
        }
    }
}

class LFTPLongestDistance : public LFTPTask
{
public:
    LFTPLongestDistance(MSTree data, int fd, std::shared_ptr<TaskGroup> taskGroup) : LFTPTask(data, fd, taskGroup) {}
    void execute()
    {
        std::ostringstream oss;
        oss << "LongestDistance: " << data_.findLongestDistance() << std::endl;
        std::string output = oss.str();
        write(fd_, output.c_str(), output.size());
    }
};

class LFTPAverageDistance : public LFTPTask
{
public:
    LFTPAverageDistance(MSTree data, int fd, std::shared_ptr<TaskGroup> taskGroup) : LFTPTask(data, fd, taskGroup) {}
    void execute()
    {
        std::ostringstream oss;
        oss << "AverageDistance: " << data_.findAverageDistance() << std::endl;
        std::string output = oss.str();
        write(fd_, output.c_str(), output.size());
    }
};

class LFTPShortestDistance : public LFTPTask
{
public:
    LFTPShortestDistance(MSTree data, int fd, std::shared_ptr<TaskGroup> taskGroup) : LFTPTask(data, fd, taskGroup) {}
    void execute()
    {
        std::ostringstream oss;
        oss << "ShortestDistance: " << data_.findShortestDistance() << std::endl;
        std::string output = oss.str();
        write(fd_, output.c_str(), output.size());
    }
};

void executeLeaderFollowerThreadPool(MSTree data, int fd)
{

    LeaderFollowerThreadPool &pool = LeaderFollowerThreadPool::getInstance(4);
    std::vector<std::shared_ptr<LFTPTask>> tasks;

    auto taskGroup = std::make_shared<TaskGroup>(3); // 5 tasks in the group

    tasks.push_back(std::make_shared<LFTPLongestDistance>(data, fd, taskGroup));
    tasks.push_back(std::make_shared<LFTPAverageDistance>(data, fd, taskGroup));
    tasks.push_back(std::make_shared<LFTPShortestDistance>(data, fd, taskGroup));
    // Add the task group to the pool
    pool.addTaskGroup(tasks);

    // Wait for all tasks in the group to complete
    taskGroup->waitForTaskGroup();

}