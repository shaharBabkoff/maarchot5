#include <unistd.h>
#include "pipeline.hpp"

// PipelineTask class implementation
PipelineTask::PipelineTask(MSTree data, int fd) : data_(data), done_(false), fd_(fd)
{
    remaining_stages_ = 0;
}

MSTree &PipelineTask::getData()
{
    return data_;
}

void PipelineTask::setData(MSTree data)
{
    data_ = data;
}

void PipelineTask::waitForCompletion()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]()
               { return done_; });
}

void PipelineTask::stageCompleted()
{
    std::unique_lock<std::mutex> lock(mutex_);
    remaining_stages_--; // Decrement the counter
    if (remaining_stages_ == 0)
    {
        done_ = true;       // All stages have completed
        cond_.notify_one(); // Notify that task processing is complete
    }
}

// TaskQueue class implementation
void TaskQueue::enqueue(std::shared_ptr<PipelineTask> task)
{
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push(task);
    cond_.notify_one(); // Notify the waiting thread
}

std::shared_ptr<PipelineTask> TaskQueue::dequeue()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]()
               { return !queue_.empty(); });
    auto task = queue_.front();
    queue_.pop();
    return task;
}

// ActiveObject class implementation
ActiveObject::ActiveObject() : done_(false) {}

ActiveObject::~ActiveObject()
{
    stop();
}

void ActiveObject::start()
{
    thread_ = std::thread(&ActiveObject::run, this);
}

void ActiveObject::stop()
{
    done_ = true;
    queue_.enqueue(nullptr); // Enqueue a null task to unblock the thread
    if (thread_.joinable())
    {
        thread_.join();
    }
}

void ActiveObject::enqueueTask(std::shared_ptr<PipelineTask> task)
{
    queue_.enqueue(task);
}

void ActiveObject::run()
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

void PLLongestDistance::processTask(std::shared_ptr<PipelineTask> task)
{
    std::ostringstream oss;
    oss << "LongestDistance: " << task->getData().findLongestDistance() << std::endl;
    std::string output = oss.str();
    write(task->getFD(), output.c_str(), output.size());
}

void PLAverageDistance::processTask(std::shared_ptr<PipelineTask> task)
{
    std::ostringstream oss;
    oss << "AverageDistance: " << task->getData().findAverageDistance() << std::endl;
    std::string output = oss.str();
    write(task->getFD(), output.c_str(), output.size());
}

void PLShortestDistance::processTask(std::shared_ptr<PipelineTask> task)
{
    std::ostringstream oss;
    oss << "ShortestDistance: " << task->getData().findShortestDistance() << std::endl;
    std::string output = oss.str();
    write(task->getFD(), output.c_str(), output.size());
}

// Pipeline class implementation
Pipeline::Pipeline() = default;

void Pipeline::addStage(ActiveObject *stage)
{
    stages_.push_back(stage);
}

void Pipeline::execute(std::shared_ptr<PipelineTask> task)
{
    task->setRemainingStages(getStageCount());
    for (auto &stage : stages_)
    {
        stage->enqueueTask(task); // Send the task to each stage independently
    }
}

void Pipeline::start()
{
    for (auto &stage : stages_)
    {
        stage->start();
    }
}

void Pipeline::stop()
{
    for (auto &stage : stages_)
    {
        stage->stop();
    }
}

int Pipeline::getStageCount() const
{
    return stages_.size();
}

Pipeline &Pipeline::getPipeline()
{
    static Pipeline instance; // Singleton instance of the pipeline
    static bool initialized = false;

    if (!initialized)
    {
        // Initialize the pipeline with stages
        instance.addStage(new PLLongestDistance());
        instance.addStage(new PLAverageDistance());
        instance.addStage(new PLShortestDistance());
        instance.start(); // Start the pipeline stages
        initialized = true;
    }
    return instance;
}
