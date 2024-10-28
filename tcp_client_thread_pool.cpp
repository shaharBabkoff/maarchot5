#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include "tcp_client_thread_pool.hpp"
#include "execute_commands.hpp"
// Thread pool class

// ThreadPool constructor
TcpClientThreadPool::TcpClientThreadPool(size_t numThreads) : stop(false)
{
    for (size_t i = 0; i < numThreads; ++i)
    {
        workers.emplace_back([this]
                             { worker(); });
    }
}

// ThreadPool destructor
TcpClientThreadPool::~TcpClientThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
    {
        worker.join();
    }
}

// Enqueue a new client task into the thread pool
void TcpClientThreadPool::enqueue(std::shared_ptr<Context> ctx)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex); // Lock for thread-safe queue access
        tasks.push(ctx);                               // Add task to the queue
    }
    condition.notify_one(); // Notify one waiting thread to process the new task
}

// Worker thread function - processes tasks from the queue
void TcpClientThreadPool::worker()
{
    while (true)
    {
        std::shared_ptr<Context> ctx;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]
                           { return stop || !tasks.empty(); });

            if (stop && tasks.empty())
                return;

            ctx = tasks.front();
            tasks.pop();
        }
        printf("in worker %d\n", ctx->fd);
        if (ctx->fd == -1)
        {
            printf("got fd==-1, releasing context memory\n");
            freeContext(ctx->context);
            ctx->context = INVALID_POINTER;
        }
        else
        {
            char buf[256]; // Buffer for client data
            printf("going to call recv!!!\n");
            int nbytes = recv(ctx->fd, buf, sizeof(buf) - 1, 0);
            printf("returned from recv!!!\n");

            if (nbytes <= 0)
            {
                // Got error or connection closed by client
                if (nbytes == 0)
                {
                    // Connection closed
                    printf("worker: socket %d hung up\n", ctx->fd);
                }
                else
                {
                    perror("recv");
                }
                close(ctx->fd); // Bye!
                freeContext(ctx->context);
                ctx->context = INVALID_POINTER;
                write(ctx->pipe_write_fd, ctx.get(), sizeof(Context));
                // write to the pipe with context==-1
            }
            else
            {
                while (nbytes > 0 && isspace((unsigned char)buf[nbytes - 1]))
                {
                    --nbytes;
                }
                buf[nbytes] = '\0';
                printf("buf: %s\n", buf);
                // Execute command received from client and update context
                executeCommandToFd(ctx->fd, buf, &ctx->context);
                write(ctx->pipe_write_fd, ctx.get(), sizeof(Context)); // write to pipe with context returned from executeCommandToFd
            }
        }
    }
}
