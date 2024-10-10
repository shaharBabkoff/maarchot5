#ifndef __EXECUTE_COMMANDS_H__
#define __EXECUTE_COMMANDS_H__
void printCommandsToFd(int fd);
void executeCommandToFd(int fd, char *command, void **context);
#endif // __EXECUTE_COMMANDS_H__