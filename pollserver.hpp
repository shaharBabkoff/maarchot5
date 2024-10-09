#ifndef __POLLSERVER_H__
#define __POLLSERVER_H__

#define INVALID_POINTER reinterpret_cast<void*>(-1)

struct Context
{
    int fd;
    int pipe_write_fd;
    void *context;
    Context(int _fd, int _pipe_write_fd, void *_context) : fd(_fd), pipe_write_fd(_pipe_write_fd), context(_context)
    {
    }
};

void poll_clients(const char *port);

#endif // __POLLSERVER_H__