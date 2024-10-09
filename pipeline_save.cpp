#include <iostream>
#include <sstream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include <atomic>
#include "MSTree.hpp"
// Task class representing the data to be processed
class Task
{
public:
    explicit Task(MSTree data, int stages, int fd) : data_(data), remaining_stages_(stages), done_(false), fd_(fd) {}

    MSTree &getData() const { return data_; }
    void setData(MSTree data) { data_ = data; }

    // Wait for the task to be processed in all stages
    void waitForCompletion()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]()
                   { return done_; });
    }

    // Called by each stage after processing to decrement the counter
    void stageCompleted()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        remaining_stages_--; // Decrement the counter
        if (remaining_stages_ == 0)
        {
            done_ = true;       // All stages have completed
            cond_.notify_one(); // Notify that task processing is complete
        }
    }

private:
    MSTree data_;
    int remaining_stages_; // Counter tracking how many stages are left
    std::mutex mutex_;
    std::condition_variable cond_;
    bool done_; // Task completion flag
    int fd_;
};

// Thread-safe task queue
class TaskQueue
{
public:
    // Enqueue a task
    void enqueue(std::shared_ptr<Task> task)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(task);
        cond_.notify_one(); // Notify the waiting thread
    }

    // Dequeue a task (waits if the queue is empty)
    std::shared_ptr<Task> dequeue()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]()
                   { return !queue_.empty(); });
        auto task = queue_.front();
        queue_.pop();
        return task;
    }

private:
    std::queue<std::shared_ptr<Task>> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

// Active Object class representing each stage of the pipeline
class ActiveObject
{
public:
    explicit ActiveObject() : done_(false) {}

    virtual ~ActiveObject() = default;

    // Start the active object thread
    void start()
    {
        thread_ = std::thread(&ActiveObject::run, this);
    }

    // Stop the active object thread (optional for cleanup)
    void stop()
    {
        done_ = true;
        queue_.enqueue(nullptr); // Enqueue a null task to unblock the thread
        if (thread_.joinable())
        {
            thread_.join();
        }
    }

    // Enqueue a task to this stage
    void enqueueTask(std::shared_ptr<Task> task)
    {
        queue_.enqueue(task);
    }

protected:
    // Process function to be implemented by subclasses
    virtual void processTask(std::shared_ptr<Task> task) = 0;

    // Run method executed by the thread
    void run()
    {
        while (!done_)
        {
            auto task = queue_.dequeue();
            if (task == nullptr)
                break;              // Exit if a null task is received
            processTask(task);      // Process the task in this stage
            task->stageCompleted(); // Notify the task that this stage is done
        }
    }

private:
    TaskQueue queue_;
    std::thread thread_;
    bool done_;
};

// LongestDistance
class LongestDistance : public ActiveObject
{
public:
    using ActiveObject::ActiveObject;

protected:
    void processTask(std::shared_ptr<Task> task) override
    {
        std::ostringstream oss;
        oss << "LongestDistance: " << task->getData().findLongestDistance() << std::endl;
        std::string output = oss.str();
        std::cout << oss.str();
        // std::cout << "Stage 1 processed task: " << task->getData() << std::endl;
    }
};

// Stage 2: Multiply the task's data by 2
class AverageDistance : public ActiveObject
{
public:
    using ActiveObject::ActiveObject;

protected:
    void processTask(std::shared_ptr<Task> task) override
    {
        std::ostringstream oss;
        oss << "AverageDistance: " << task->getData().findAverageDistance() << std::endl;
        std::string output = oss.str();
        std::cout << oss.str();
    }
};

// Stage 3: Print the result
class ShortestDistance : public ActiveObject
{
public:
    using ActiveObject::ActiveObject;

protected:
    void processTask(std::shared_ptr<Task> task) override
    {
        std::ostringstream oss;
        oss << "ShortestDistance: " << task->getData().findShortestDistance() << std::endl;
        std::string output = oss.str();
        std::cout << oss.str();
    }
};

// Pipeline class holding the stages
class Pipeline
{
public:
    // Add a stage to the pipeline
    void addStage(ActiveObject *stage)
    {
        stages_.push_back(stage);
    }

    // Execute a task through the pipeline (independently in all stages)
    void execute(std::shared_ptr<Task> task)
    {
        for (auto &stage : stages_)
        {
            stage->enqueueTask(task); // Send the task to each stage independently
        }
    }

    // Start all stages
    void start()
    {
        for (auto &stage : stages_)
        {
            stage->start();
        }
    }

    // Stop all stages
    void stop()
    {
        for (auto &stage : stages_)
        {
            stage->stop();
        }
    }

    // Get the number of stages in the pipeline
    int getStageCount() const
    {
        return stages_.size();
    }
    static Pipeline &getPipeline()
    {
        static Pipeline instance; // Singleton instance of the pipeline
        static bool initialized = false;

        if (!initialized)
        {
            // Initialize the pipeline with stages
            instance.addStage(new LongestDistance());
            instance.addStage(new AverageDistance());
            instance.addStage(new ShortestDistance());
            instance.start(); // Start the pipeline stages
            initialized = true;
        }
        return instance;
    }

private:
    Pipeline() = default;                           // Private constructor for singleton pattern
    Pipeline(const Pipeline &) = delete;            // Prevent copying
    Pipeline &operator=(const Pipeline &) = delete; // Prevent assignment
    std::vector<ActiveObject *> stages_;            // Hold the stages
};

