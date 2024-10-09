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

class Task
{
public:
    explicit Task(MSTree data, int stages, int fd) : data_(data), remaining_stages_(stages), done_(false), fd_(fd) {}

    MSTree &getData()  { return data_; }
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

class ActiveObject;

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

    static Pipeline& getPipeline() {
        static Pipeline instance;  // Singleton instance of the pipeline
        static bool initialized = false;

        if (!initialized) {
            // Initialize the pipeline with stages
            instance.addStage(new LongestDistance());
            instance.addStage(new AverageDistance());
            instance.addStage(new ShortestDistance());
            instance.start();  // Start the pipeline stages
            initialized = true;
        }
        return instance;
    }
private:
    Pipeline() = default;  // Private constructor for singleton pattern
    Pipeline(const Pipeline&) = delete;  // Prevent copying
    Pipeline& operator=(const Pipeline&) = delete;  // Prevent assignment
    std::vector<ActiveObject *> stages_; // Hold the stages
};

#endif  // PIPELINE_HPP
