#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include "LineParser.h"

#define MAX_LINE 2048

int bugMode = 0;
__pid_t pid;
cmdLine *pCmdLine = NULL;
int blockMode = 0;

int isBugMode(int argc, char **argv)
{
    char *bugMode = "-D";
    int i = 1;
    for (i; i < argc; i++)
    {
        if (strcmp(argv[i], bugMode) == 0)
        {
            return 1;
        }
    }
    return 0;
}

execute(cmdLine *pCmdLine)
{
    char **args;
    args = pCmdLine -> arguments;
    char *command = args[0];
    if (strcmp(command, "cd") == 0)
    {
        exit(1); 
    }

    printCommand(command);
    execvp(command, args);
}

void printDirectory()
{
    char buf[PATH_MAX];
    getcwd(buf, PATH_MAX);
    printf("directory name is: %s\n", buf);
}

void printPid(__pid_t PID)
{
    if (bugMode)
    {
        fprintf(stderr, "PID is: %d\n", pid);
    }
}

void printCommand(char *str)
{
    if (bugMode)
    {
        fprintf(stderr, "command is: %s\n", str);
    }
}

void checkIfNeedToWait(__pid_t pid)
{
    if (blockMode)
    {
        printf("Waiting to child termination\n");
        waitpid(pid, NULL, 0);
    }
}

checkIfNeedToCd(cmdLine *pCmdLine)
{
    char **args;
    args = pCmdLine->arguments;
    char *command = args[0];
    char *path = args[1];
    if (strcmp(command, "cd") == 0)
    {
        int check = chdir(path);
        if (check == -1)
        {
            perror("cd failed");
        }
    }
}

void shouldExit(char *str)
{
    if (strcmp(str, "quit") == 0)
    {
        //freeCmdLines(pCmdLine);
        printf("Exit Program\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    bugMode = isBugMode(argc, argv);
   
    char input[MAX_LINE];
    while (1)
    {
        printDirectory();
        fgets(input, MAX_LINE, stdin);
        input[strlen(input) - 1] = 0;
        shouldExit(input);
        pCmdLine = parseCmdLines(input);
        blockMode = pCmdLine->blocking;
        checkIfNeedToCd(pCmdLine);
        if (!(pid = fork()))
        {
            sleep(1);
            printPid(pid);
            execute(pCmdLine);
            perror("Error: ");
            exit(1);
        }
        else
        {
            checkIfNeedToWait(pid);
            printPid(pid);
            freeCmdLines(pCmdLine);
        }
    }
}