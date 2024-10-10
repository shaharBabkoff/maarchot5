#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "tcp_dup.hpp"
#include "Graph.hpp"
#include "MSTStrategy.hpp"
#include "MSTree.hpp"
#include "pipeline.hpp"
// #include "kosaraju.h"
#define COMMANDS_USAGE "enter one of the following commands:\n"                              \
                       "            Newgraph <verttices>,<edges>\n"                          \
                       "                User should enter <edges> pairs of directed edges\n" \
                       "            Newedge <from>,<to>,<weight>\n"                          \
                       "            Removeedge <from>,<to>\n"                                \
                       "            Print\n"                                                 \
                       "            Prim\n"                                                  \
                       "            Kruskal\n\n"                                             \
                       "enter command:\n"

#define MISSING_VERT_EDGE "Must specify verttices and edges\n"

#define PRINT_EDGES_MESSAGE \
    "Enter the  directed edges as triplets of vertices <from>,<to>,<weight>:\n"

#define MISSING_GRAPH "Graph does not exist, please create a graph\n"

#define INVALID_NEW_EDGE "Must specify <from>,<to>,<weight> of the new edge\n"

#define INVALID_EDGE "Must specify both endpoints of the edge to remove\n"

#define ILLEGAL_COMMAND "unrecognized command "

#define ENTER_COMMAND "Enter command:\n"

#define NEWLINE "\n"

void printCommands(int fd)
{
    write(fd, COMMANDS_USAGE, sizeof(COMMANDS_USAGE));
}

void getParameters(char **param1, char **param2, char **param3, char **saveptr)
{
    char *input = strtok_r(NULL, " ", saveptr);

    *param1 = strtok_r(input, ",\n", saveptr);
    *param2 = strtok_r(NULL, ",", saveptr);
    if (param3 != NULL)
    {
        *param3 = strtok_r(NULL, ",", saveptr);
    }
}

Graph *getNewGraph(int fd, int vertices, int edges)
{
    Graph *graph = new Graph(vertices);
    if (edges > 0)
    {
        write(fd, PRINT_EDGES_MESSAGE, sizeof(PRINT_EDGES_MESSAGE));
        for (int i = 0; i < edges; i++)
        {
            char input[1024];
            ssize_t bytesRead;
            bytesRead = read(fd, input, sizeof(input) - 1);
            if (bytesRead < 0)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }
            input[bytesRead] = '\0';

            char *src, *dest, *weight, *saveptr;
            src = strtok_r(input, ",\n", &saveptr);
            dest = strtok_r(NULL, ",\n", &saveptr);
            weight = strtok_r(NULL, ",\n", &saveptr);
            if (src == NULL || dest == NULL || weight == NULL)
            {
                write(fd, INVALID_NEW_EDGE, sizeof(INVALID_NEW_EDGE));
                delete graph;
                graph = NULL;
                break;
            }
            graph->addEdge(atoi(src), atoi(dest), atof(weight));
        }
    }
    return graph;
}

void execute(int fd, char *token, Graph *graph)
{
    MSTFactory factory;
    std::unique_ptr<MSTStrategy> strategy;
    MSTree mst;
    if (strcmp(token, "Prim") == 0)
    {
        strategy = factory.getMSTStrategy(MSTFactory::PRIM);
    }
    else if (strcmp(token, "Kruskal") == 0)
    {
        strategy = factory.getMSTStrategy(MSTFactory::KRUSKAL);
    }
    else
    {
        // write error
        return;
    }
    mst = strategy->computeMST(*graph);
    mst.printMST(fd);
    Pipeline &pipeline = Pipeline::getPipeline();
    auto task = std::make_shared<Task>(mst, fd);
    pipeline.execute(task);
    task->waitForCompletion();
}

void executeCommand(int fd, char *input, void **context)
{
    char *param1 = NULL, *param2 = NULL, *param3 = NULL, *saveptr;

    char *token = strtok_r(input, " \n", &saveptr);
    Graph *graph = (Graph *)(*context);
    if (token != NULL)
    {
        if (strcmp(token, "Newgraph") == 0)
        {
            if (graph != NULL)
            {
                delete graph;
                graph = NULL;
            }
            getParameters(&param1, &param2, NULL, &saveptr);
            if (param1 == NULL || param2 == NULL)
            {
                write(fd, MISSING_VERT_EDGE, sizeof(MISSING_VERT_EDGE));
            }
            else
            {
                graph = getNewGraph(fd, atoi(param1), atoi(param2));
                *context = graph;
            }
        }
        else if (strcmp(token, "Prim") == 0 || strcmp(token, "Kruskal") == 0)
        {
            printf("%s....\n", token);
            if (graph != NULL)
            {
                execute(fd, token, graph);
                // graph->printGraph(fd);
            }
            else
            {
                write(fd, MISSING_GRAPH, sizeof(MISSING_GRAPH));
            }
        }
        else if (strcmp(token, "Print") == 0)
        {
            printf("Print....\n");
            if (graph != NULL)
            {
                graph->printGraph(fd);
            }
            else
            {
                write(fd, MISSING_GRAPH, sizeof(MISSING_GRAPH));
            }
        }
        else if (strcmp(token, "Newedge") == 0)
        {
            printf("Newedge....\n");
            if (graph != NULL)
            {
                getParameters(&param1, &param2, &param3, &saveptr);

                if (param1 == NULL || param2 == NULL || param3 == NULL)
                {
                    write(fd, INVALID_NEW_EDGE, sizeof(INVALID_NEW_EDGE));
                }
                else
                {

                    graph->addEdge(atoi(param1), atoi(param2), atof(param3));
                }
            }
            else
            {
                write(fd, MISSING_GRAPH, sizeof(MISSING_GRAPH));
            }
        }
        else if (strcmp(token, "Removeedge") == 0)
        {
            printf("Removeedge....\n");
            if (graph != NULL)
            {
                getParameters(&param1, &param2, NULL, &saveptr);

                if (param1 == NULL || param2 == NULL)
                {
                    write(fd, INVALID_EDGE, sizeof(INVALID_EDGE));
                }
                else
                {

                    graph->removeEdge(atoi(param1), atoi(param2));
                }
            }
            else
            {
                write(fd, MISSING_GRAPH, sizeof(MISSING_GRAPH));
            }
        }
        else
        {
            write(fd, ILLEGAL_COMMAND, sizeof(ILLEGAL_COMMAND));
            write(fd, token, strlen(token));
            write(fd, NEWLINE, sizeof(NEWLINE));
        }
    }
    write(fd, ENTER_COMMAND, sizeof(ENTER_COMMAND));
}

int tcp_dup_std(int fd, int std)
{
    int saved_stdout = dup(std);
    if (saved_stdout < 0)
    {
        perror("dup");
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, std) < 0)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    return saved_stdout;
}

void tcp_undup_std(int saved_stdout, int std)
{
    if (dup2(saved_stdout, std) < 0)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
}

void printCommandsToFd(int fd)
{
    // int saved_stdout = tcp_dup_std(fd, STDOUT_FILENO);
    // int saved_stdin = tcp_dup_std(fd, STDIN_FILENO);
    printCommands(fd);
    // tcp_undup_std(saved_stdout, STDOUT_FILENO);
    // tcp_undup_std(saved_stdin, STDIN_FILENO);
}

void executeCommandToFd(int fd, char *command, void **context)
{
    // int saved_stdout = tcp_dup_std(fd, STDOUT_FILENO);
    // int saved_stdin = tcp_dup_std(fd, STDIN_FILENO);
    executeCommand(fd, command, context);
    // tcp_undup_std(saved_stdout, STDOUT_FILENO);
    // tcp_undup_std(saved_stdin, STDIN_FILENO);
}
