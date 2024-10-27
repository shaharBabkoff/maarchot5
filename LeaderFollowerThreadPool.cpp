#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sstream>
#include "LeaderFollowerThreadPool.hpp"
using namespace std;
// TaskGroup class implementation
TaskGroup::TaskGroup(size_t taskCount)
    : counter(make_shared<atomic<int>>(taskCount)) {}

void TaskGroup::waitForTaskGroup()
{
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [this]
            { return *counter == 0; });
}

void TaskGroup::taskCompleted()
{
    if (--(*counter) == 0)
    {
        lock_guard<mutex> lock(mtx);
        cv.notify_one();
    }
}

// LFTPTask class implementation
LFTPTask::LFTPTask(MSTree data, int fd, shared_ptr<TaskGroup> taskGroup)
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
        threads.emplace_back(&LeaderFollowerThreadPool::workerThread, this, i);
    }
}

LeaderFollowerThreadPool::~LeaderFollowerThreadPool()
{
    {
        unique_lock<mutex> lock(mtx);
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

void LeaderFollowerThreadPool::addTaskGroup(const vector<shared_ptr<LFTPTask>> &tasks)
{

    unique_lock<mutex> lock(mtx);
    for (const auto &task : tasks)
    {
        taskQueue.push(task);
    }
    condVar.notify_all(); // Notify threads that new tasks are available
}

void LeaderFollowerThreadPool::workerThread(int threadId) {
    while (true) {
        shared_ptr<LFTPTask> task;

        {
            unique_lock<mutex> lock(mtx);

            // Only the first thread to enter when currentLeader == -1 will set itself as the leader
            while (currentLeader != threadId && !stopPool) {
                if (currentLeader == -1) {
                    // No leader exists, so this thread becomes the leader
                    currentLeader = threadId;
                } else {
                    // Wait for current leader to finish or stop signal
                    condVar.wait(lock);
                }
            }

            if (stopPool) {
                break;
            }

            // Leader waits for a task to be available in the queue
            condVar.wait(lock, [this] { return !taskQueue.empty() || stopPool; });

            if (stopPool) {
                break;
            }

            // As the leader, pick the next task if available
            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
            }

            // Pass leadership if there are other threads waiting
            currentLeader = -1;
            condVar.notify_all();  // Notify waiting threads to check for leadership
        }

        // Process the task outside the critical section
        if (task) {
            task->process();  // Process with current thread's ID
        }
    }
}

class LFTPTotalWeight : public LFTPTask
{
public:
    LFTPTotalWeight(MSTree data, int fd, shared_ptr<TaskGroup> taskGroup) : LFTPTask(data, fd, taskGroup) {}
    void execute()
    {
        ostringstream oss;
        oss << "TotalWeight: " << data_.getTotalWeight() << endl;
        string output = oss.str();
        write(fd_, output.c_str(), output.size());
    }
};

class LFTPLongestDistance : public LFTPTask
{
public:
    LFTPLongestDistance(MSTree data, int fd, shared_ptr<TaskGroup> taskGroup) : LFTPTask(data, fd, taskGroup) {}
    void execute()
    {
        ostringstream oss;
        oss << "LongestDistance: " << data_.findLongestDistance() << endl;
        string output = oss.str();
        write(fd_, output.c_str(), output.size());
    }
};

class LFTPAverageDistance : public LFTPTask
{
public:
    LFTPAverageDistance(MSTree data, int fd, shared_ptr<TaskGroup> taskGroup) : LFTPTask(data, fd, taskGroup) {}
    void execute()
    {
        ostringstream oss;
        oss << "AverageDistance: " << data_.findAverageDistance() << endl;
        string output = oss.str();
        write(fd_, output.c_str(), output.size());
    }
};

class LFTPShortestDistance : public LFTPTask
{
public:
    LFTPShortestDistance(MSTree data, int fd, shared_ptr<TaskGroup> taskGroup) : LFTPTask(data, fd, taskGroup) {}
    void execute()
    {
        ostringstream oss;
        oss << "ShortestDistance: " << data_.findShortestDistance() << endl;
        string output = oss.str();
        write(fd_, output.c_str(), output.size());
    }
};

void executeLeaderFollowerThreadPool(MSTree data, int fd)
{
    LeaderFollowerThreadPool &pool = LeaderFollowerThreadPool::getInstance(4);
    vector<shared_ptr<LFTPTask>> tasks;
    auto taskGroup = make_shared<TaskGroup>(4);
    tasks.push_back(make_shared<LFTPTotalWeight>(data, fd, taskGroup));
    tasks.push_back(make_shared<LFTPLongestDistance>(data, fd, taskGroup));
    tasks.push_back(make_shared<LFTPAverageDistance>(data, fd, taskGroup));
    tasks.push_back(make_shared<LFTPShortestDistance>(data, fd, taskGroup));
    // Add the task group to the pool
    pool.addTaskGroup(tasks);
    // Wait for all tasks in the group to complete
    taskGroup->waitForTaskGroup();
}