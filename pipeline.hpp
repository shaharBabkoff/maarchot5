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
// PipelineTask class representing the data to be processed
class PipelineTask
{
private:
    MSTree data_;
    int remaining_stages_; // Counter tracking how many stages are left
    std::mutex mutex_;
    std::condition_variable cond_;
    bool done_; // PipelineTask completion flag
    int fd_;

public:
    explicit PipelineTask(MSTree data, int fd);

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
};

// Thread-safe task queue
class TaskQueue
{
private:
    std::queue<std::shared_ptr<PipelineTask>> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;

public:
    // Enqueue a task
    void enqueue(std::shared_ptr<PipelineTask> task);

    // Dequeue a task (waits if the queue is empty)
    std::shared_ptr<PipelineTask> dequeue();
};

// Active Object class representing each stage of the pipeline
class ActiveObject
{

private:
    TaskQueue queue_;
    std::thread thread_;
    bool done_;
    ActiveObject *next_stage_; // Pointer to the next stage in the pipeline
protected:
    // Process function to be implemented by subclasses
    virtual void processTask(std::shared_ptr<PipelineTask> task) = 0;

public:
    explicit ActiveObject(ActiveObject *next_stage = nullptr);
    virtual ~ActiveObject();

    // Start the active object thread
    void start();

    // Stop the active object thread (optional for cleanup)
    void stop();

    // Enqueue a task to this stage
    void enqueueTask(std::shared_ptr<PipelineTask> task);

    void setNextStage(ActiveObject *next_stage)
    {
        next_stage_ = next_stage;
    }

    // Run method executed by the thread
    void run();
};
class PLTotalWeight : public ActiveObject
{
public:
    using ActiveObject::ActiveObject;

protected:
    void processTask(std::shared_ptr<PipelineTask> task) override;
};
class PLLongestDistance : public ActiveObject
{
public:
    using ActiveObject::ActiveObject;

protected:
    void processTask(std::shared_ptr<PipelineTask> task) override;
};

class PLAverageDistance : public ActiveObject
{
public:
    using ActiveObject::ActiveObject;

protected:
    void processTask(std::shared_ptr<PipelineTask> task) override;
};

class PLShortestDistance : public ActiveObject
{
public:
    using ActiveObject::ActiveObject;

protected:
    void processTask(std::shared_ptr<PipelineTask> task) override;
};

// Pipeline class holding the stages
class Pipeline
{
private:
    Pipeline();                                     // Private constructor for singleton pattern
    Pipeline(const Pipeline &) = delete;            // Prevent copying
    Pipeline &operator=(const Pipeline &) = delete; // Prevent assignment

    std::vector<ActiveObject *> stages_; // Hold the stages
public:
    // Add a stage to the pipeline
    void addStage(ActiveObject *stage);

    // Execute a task through the pipeline (independently in all stages)
    void execute(std::shared_ptr<PipelineTask> task);

    // Start all stages
    void start();

    // Stop all stages
    void stop();

    // Get the number of stages in the pipeline
    int getStageCount() const;

    // Singleton pattern: Get the pipeline instance (initialized with stages)
    static Pipeline &getPipeline();
};

#endif // PIPELINE_HPP
