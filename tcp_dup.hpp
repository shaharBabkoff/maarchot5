#ifndef __TCP_DUP_H__
#define __TCP_DUP_H__
void printCommandsToFd(int fd);
void executeCommandToFd(int fd, char *command, void **context);
#endif // __TCP_DUP_H__