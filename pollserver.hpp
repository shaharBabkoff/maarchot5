#ifndef __POLLSERVER_H__
#define __POLLSERVER_H__

#define INVALID_POINTER reinterpret_cast<void*>(-1)
// Context struct representing client specific dada
struct Context
{
    int fd; // File descriptor for the client connection
    int pipe_write_fd; // Pipe descriptor for communication with thread pool
    void *context;  // Custom context pointer for additional client data
    Context(int _fd, int _pipe_write_fd, void *_context) : fd(_fd), pipe_write_fd(_pipe_write_fd), context(_context)
    {
    }
};
// Main function to start the poll server and handle clients on the specified port
void poll_clients(const char *port);

#endif // __POLLSERVER_H__