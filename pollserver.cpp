/*
** pollserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include "pollserver.hpp"
#include "listner.hpp"
#include "tcp_dup.hpp"
#include "tcp_client_thread_pool.hpp"
// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size)
    {
        *fd_size *= 2; // Double it

        *pfds = (struct pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
    printf("add_to_pfds:  %d\n", newfd);
}

void add_to_contexts(struct Context *ctxs[], struct Context *ctx, int *context_count, int *context_size)
{
    bool inList = false;
    // check if fd is in the list
    for (int i = 0; i < *context_count; i++)
    {
        if ((*ctxs)[i].fd == ctx->fd)
        {
            inList = true;
            (*ctxs)[i].context = ctx->context;
            break;
        }
    }
    // If we don't have room, add more space in the pfds array
    if (!inList)
    {
        if (*context_count == *context_size)
        {
            *context_size *= 2; // Double it

            *ctxs = (struct Context *)realloc(*ctxs, sizeof(**ctxs) * (*context_size));
        }

        (*ctxs)[*context_count] = *ctx;

        (*context_count)++;
    }
}

void *get_context(Context ctxs[], int fd, int *context_count)
{
    void *context = NULL;
    for (int i = 0; i < *context_count; i++)
    {
        if (ctxs[i].fd == fd)
        {
            context = ctxs[i].context;
            break;
        }
    }
    return context;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    printf("going to remove %d\n", pfds[i].fd);
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}
// del_from_contexts(contexts, pfds[i].fd, &context_count);
void del_from_contexts(Context ctxs[], int fd, int *context_count)
{
    bool inList = false;
    int i = 0;
    // check if fd is in the list
    for (i = 0; i < *context_count; i++)
    {
        if ((ctxs)[i].fd == fd)
        {
            inList = true;
            break;
        }
    }

    if (inList)
    {
        ctxs[i] = ctxs[*context_count - 1];
        (*context_count)--;
    }
}

void poll_clients(const char *port)
{
    int listener; // Listening socket descriptor
    int pipefds[2];
    int newfd;                          // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    // char buf[256]; // Buffer for client data

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections
    // (We'll realloc as necessary)

    if (pipe(pipefds) == -1)
    {
        fprintf(stderr, "error creating pipe\n");
        exit(EXIT_FAILURE);
    }

    int fd_count = 0;
    int fd_size = 7;
    int context_count = 0;
    int context_size = 5;

    // Set up and get a listening socket
    listener = createListner(port);

    if (listener == -1)
    {
        fprintf(stderr, "error getting listening socket\n");
        exit(EXIT_FAILURE);
    }

    struct pollfd *pfds = (struct pollfd *)malloc(sizeof *pfds * fd_size);
    struct Context *contexts = (struct Context *)malloc(sizeof *contexts * context_size);

    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection
    pfds[0].revents = 0;
    pfds[1].fd = pipefds[0];
    pfds[1].events = POLLIN; // Report client action completed
    pfds[1].revents = 0;

    fd_count = 2; // For the listener and pipe

    // Main loop
    TcpClientThreadPool tcpClientThreadPool(4);
    for (;;)
    {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1)
        {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for (int i = 0; i < fd_count; i++)
        {

            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN)
            { // We got one!!

                if (pfds[i].fd == listener)
                {
                    // If listener is ready to read, handle new connection

                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                                   (struct sockaddr *)&remoteaddr,
                                   &addrlen);

                    if (newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("pollserver: new connection from %s on "
                               "socket %d\n",
                               inet_ntop(remoteaddr.ss_family,
                                         get_in_addr((struct sockaddr *)&remoteaddr),
                                         remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                        printCommandsToFd(newfd);
                    }
                }
                else if (pfds[i].fd == pipefds[0])
                {
                    printf("completed client operation.going to read from pipe\n");
                    // read context from pipe, reinsert fd to array
                    struct Context ctx(-1, -1, NULL);
                    read(pfds[i].fd, &ctx, sizeof(ctx));
                    if (ctx.context == INVALID_POINTER)
                    {
                        del_from_contexts(contexts, ctx.fd, &context_count);
                    }
                    else
                    {
                        add_to_contexts(&contexts, &ctx, &context_count, &context_size);
                        add_to_pfds(&pfds, ctx.fd, &fd_count, &fd_size);
                    }
                }
                else
                {

                    // If not the listener, we're just a regular client
                    // find context if exists:
                    printf("ready to read from %d, going to post to thread pool!!!\n", pfds[i].fd);
                    tcpClientThreadPool.enqueue(std::make_shared<Context>(pfds[i].fd, pipefds[1], get_context(contexts, pfds[i].fd, &context_count)));
                    del_from_pfds(pfds, i, &fd_count);
                    // int nbytes = recv(pfds[i].fd, buf, sizeof(buf) - 1, 0);
                    // printf("returned from recv!!!\n");
                    // int sender_fd = pfds[i].fd;

                    // if (nbytes <= 0)
                    // {
                    //     // Got error or connection closed by client
                    //     if (nbytes == 0)
                    //     {
                    //         // Connection closed
                    //         printf("pollserver: socket %d hung up\n", sender_fd);
                    //     }
                    //     else
                    //     {
                    //         perror("recv");
                    //     }
                    //     del_from_contexts(contexts, pfds[i].fd, &context_count);
                    //     close(pfds[i].fd); // Bye!

                    //     del_from_pfds(pfds, i, &fd_count);
                    // }
                    // else
                    // {
                    //     while (nbytes > 0 && isspace((unsigned char)buf[nbytes - 1]))
                    //     {
                    //         --nbytes;
                    //     }
                    //     buf[nbytes] = '\0';
                    //     executeCommandToFd(sender_fd, buf);
                    // }
                }
            }
        }
    }
}