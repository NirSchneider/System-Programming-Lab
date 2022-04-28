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

char *strClone(const char *source)
{
    char* clone = (char*)malloc(strlen(source) + 1);
    strcpy(clone, source);
    return clone;
}

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
    dup2(fdOutput, STDOUT_FILENO);
    dup2(fdInput, STDIN_FILENO);
    close(fdInput);
    close(fdOutput);
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

int checkIfNeedToCd(cmdLine *pCmdLine)
{
    char **args;
    args = pCmdLine->arguments;
    char *command = args[0];
    char *path = args[1];
    if (strcmp(command, "cd") == 0)
    {
        if (strcmp(path, "~") == 0)
        {
            cdToHomDir();
            return 1;
        }
        int check = chdir(path);
        if (check == -1)
        {
            perror("Failed to cd");
            return 1;
        }
        return 1;
    }
    return 0;
}

void shouldExit(char *str)
{
    if (strcmp(str, "quit") == 0)
    {
        printf("Exit Program\n");
        exit(1);
    }
}


char *getValue(vars **linkVars, char *name)
{
    vars *temp = *linkVars;
    while (temp != NULL)
    {
        if (strcmp(temp->name, name) == 0)
        {
            return temp->value;
        }
        temp = temp->next;
    }
    return NULL;
}
void replaceVals(cmdLine *cmd)
{
    char **args = pCmdLine->arguments;
    int size = pCmdLine->argCount;
    for (int i = 0; i < size; i++)
    {
        if (args[i][0] == '$')
        {
            char *value = getValue(&linkVars, args[i] + 1);
            printf("the string is %s\n", value);
            if (value == NULL)
            {
                fprintf(stderr, "Varibale %s not exist\n", args[i] + 1);
                return;
            }
            replaceCmdArg(cmd, i, value);
        }
    }
}

void cdToHomDir()
{
    char *homedir = getenv("HOME");
    int check = chdir(homedir);
    if (check == -1)
    {
        perror("Failed to cd to HomeDir\n");
    }
}

int main(int argc, char *argv[])
{
    bugMode = isBugModeOn(argc, argv);
    char input[MAX_LINE];
startWhile:
    while (1)
    {
        pid_t pid2 = 0;
        int fd[2];
        int pipeOn = 0;
        printDirectory();
        fgets(input, MAX_LINE, stdin);
        input[strlen(input) - 1] = 0; //clean the \n
        shouldExit(input);
        pCmdLine = parseCmdLines(input);
        replaceVals(pCmdLine);
        if (pCmdLine->next != NULL)
        {
            pipeOn = 1;
            pipe(fd);
        }

        blockMode = pCmdLine->blocking; // & in end input - 0 , 1

        int check = checkIfNeedToCd(pCmdLine);
        if (check)
        {
            freeCmdLines(pCmdLine);
            goto startWhile;
        }
        if (!(pid = fork()))
        {
            if (pipeOn)
            {
                close(STDOUT_FILENO);
                dup(fd[1]);
                close(fd[1]);
            }
            sleep(1); // to check if the waiting work.
            printPid(pid);
            execute(pCmdLine);
            perror("Error: "); // if we get here its becouse execute failed.
            exit(1);           //kill child
        }
        //Parentcd
        else
        {
            if (pipeOn)
            {
                close(fd[1]);
                pid2 = fork();
                if (!pid2)
                {
                    close(STDIN_FILENO);
                    dup(fd[0]);
                    close(fd[0]);
                    printPid(pid2);
                    execute(pCmdLine->next);
                    perror("Error: "); // if we get here its becouse execute failed.
                    exit(1);           //kill child
                }
                //parent
                else
                {
                    close(fd[0]);
                }
            }
            checkIfNeedToWait(pid);
            if (pid2 != 0)
                checkIfNeedToWait(pid2);
            printPid(pid);
            freeCmdLines(pCmdLine);
        }
    }
}