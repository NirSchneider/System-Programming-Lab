#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h" //change it to .h when use makefile
#include <fcntl.h>
#define MAX_LINE 2048
//globals
int bugMode = 0;
__pid_t pid;
cmdLine *pCmdLine = NULL;
int blockMode = 0;

typedef struct vars
{
    char *name;
    char *value;
    struct vars *next;
} vars;

vars *linkVars = NULL;

int isBugModeOn(int argc, char **argv)
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
    args = pCmdLine->arguments;
    char *command = args[0];
    if (strcmp(command, "cd") == 0)
    {
        exit(1); //The parent do it.
    }
    printCommand(command);
    int fdOutput = open(pCmdLine->outputRedirect, O_RDWR, 0600);
    int fdInput = open(pCmdLine->inputRedirect, O_RDWR, 0600);
    if (fdInput > 0) {
        close(STDIN_FILENO);
        dup(fdInput);
    }
    if (fdOutput > 0) {
        close(STDOUT_FILENO);
        dup(fdOutput);
    }
    execvp(command, args);
}
void printDirectory()
{
    char buf[PATH_MAX];
    getcwd(buf, PATH_MAX);
    printf("The working directory is: %s\n", buf);
}

void printPid(__pid_t PID)
{
    if (bugMode)
    {
        fprintf(stderr, "The PID is: %d\n", pid);
    }
}

void printCommand(char *str)
{
    if (bugMode)
    {
        fprintf(stderr, "The command is: %s\n", str);
    }
}

void checkIfNeedToWait(__pid_t pid)
{
    if (blockMode)
    {
        printf("Waiting for Child to Finish\n");
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
            perror("Failed to cd");
        }
    }
}

void shouldExit(char *str)
{
    if (strcmp(str, "quit") == 0)
    {
        printf("Exit Program\n");
        exit(1);
    }
}


int main(int argc, char *argv[])
{
    bugMode = isBugModeOn(argc, argv);

    char input[MAX_LINE];
    while (1)
    {
        printDirectory();
        fgets(input, MAX_LINE, stdin);
        input[strlen(input) - 1] = 0; //clean the \n
        shouldExit(input);
        pCmdLine = parseCmdLines(input);
        blockMode = pCmdLine->blocking; // & in end input - 0 , 1
        checkIfNeedToCd(pCmdLine);
        //printf("The blocking mode is: %d\n",blockMode);
        //Child
        if (!(pid = fork()))
        {
            sleep(1); // to check if the waiting work.
            printPid(pid);
            execute(pCmdLine);
            perror("Error: "); // if we get here its becouse execute failed.
            exit(1);           //kill child
        }
        //Parent
        else
        {
            checkIfNeedToWait(pid);
            printPid(pid);
            freeCmdLines(pCmdLine);
        }
    }
}