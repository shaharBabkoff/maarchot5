#ifndef __EXECUTE_COMMANDS_H__
#define __EXECUTE_COMMANDS_H__
#define INVALID_POINTER reinterpret_cast<void*>(-1)

void printCommandsToFd(int fd);
void freeContext(void *context);
void executeCommandToFd(int fd, char *command, void **context);
#endif // __EXECUTE_COMMANDS_H__