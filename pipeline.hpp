#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include <atomic>
#include <sstream>
#include "MSTree.hpp"
// Task class representing the data to be processed
class Task
{
public:
    explicit Task(MSTree data, int fd);

    MSTree &getData();
    void setData(MSTree data);

    int getFD()
    {
        return fd_;
    }
    // Wait for the task to be processed in all stages
    void waitForCompletion();

    // Called by each stage after processing to decrement the counter
    void stageCompleted();

    void setRemainingStages(int remaining_stages)
    {
        remaining_stages_ = remaining_stages;
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
    void enqueue(std::shared_ptr<Task> task);

    // Dequeue a task (waits if the queue is empty)
    std::shared_ptr<Task> dequeue();

private:
    std::queue<std::shared_ptr<Task>> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

// Active Object class representing each stage of the pipeline
class ActiveObject
{
public:
    explicit ActiveObject();
    virtual ~ActiveObject();

    // Start the active object thread
    void start();

    // Stop the active object thread (optional for cleanup)
    void stop();

    // Enqueue a task to this stage
    void enqueueTask(std::shared_ptr<Task> task);

protected:
    // Process function to be implemented by subclasses
    virtual void processTask(std::shared_ptr<Task> task) = 0;

private:
    TaskQueue queue_;
    std::thread thread_;
    bool done_;

    // Run method executed by the thread
    void run();
};

// Stage 1: A dummy stage to simulate processing
class LongestDistance : public ActiveObject
{
protected:
    void processTask(std::shared_ptr<Task> task) override;
};

// Stage 2: Multiply the task's data by 2
class AverageDistance : public ActiveObject
{
protected:
    void processTask(std::shared_ptr<Task> task) override;
};

// Stage 3: Print the result
class ShortestDistance : public ActiveObject
{
protected:
    void processTask(std::shared_ptr<Task> task) override;
};

// Pipeline class holding the stages
class Pipeline
{
public:
    // Add a stage to the pipeline
    void addStage(ActiveObject *stage);

    // Execute a task through the pipeline (independently in all stages)
    void execute(std::shared_ptr<Task> task);

    // Start all stages
    void start();

    // Stop all stages
    void stop();

    // Get the number of stages in the pipeline
    int getStageCount() const;

    // Singleton pattern: Get the pipeline instance (initialized with stages)
    static Pipeline &getPipeline();

private:
    Pipeline();                                     // Private constructor for singleton pattern
    Pipeline(const Pipeline &) = delete;            // Prevent copying
    Pipeline &operator=(const Pipeline &) = delete; // Prevent assignment

    std::vector<ActiveObject *> stages_; // Hold the stages
};

#endif // PIPELINE_HPP
