#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 2048
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process
{
    cmdLine *cmd;
    __pid_t pid;
    int status;
    struct process *next;

}process;

int bugMode = 0;
__pid_t pid = 1;
cmdLine *pCmdLine = NULL;
int blockMode = 0;
process *process_list = NULL;
int status;
int isStop;
int isResumed;
int isTerminated;

//task1
int isBugMode(int argc, char **argv);
void execute(cmdLine *pCmdLine);
void printDirectory();
void printPid(__pid_t PID);
void printCommand(char *str);
void checkIfNeedToWait(__pid_t pid);
checkIfNeedToCd(cmdLine *pCmdLine);
shouldExit(char *str);

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

void execute(cmdLine *pCmdLine)
{
    char **args;
    args = pCmdLine->arguments;
    char *command = args[0];
    if (strcmp(command, "cd") == 0)
    {
        exit(1);
    }

    if (strcmp(command, "procs") == 0)
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
    printf("The directory is: %s\n", buf);
}

void printPid(__pid_t PID)
{
    if (bugMode)
    {
        fprintf(stderr, "PID: %d\n", pid);
    }
}

void printCommand(char *str)
{
    if (bugMode)
    {
        fprintf(stderr, "command: %s\n", str);
    }
}

void checkIfNeedToWait(__pid_t pid)
{
    if (blockMode)
    {
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
        int check = chdir(path);
        if (check == -1)
        {
            perror("cd failed");
        }
        return 1;
    }

    return 0;
}

int checkIfNeedToProcs(cmdLine *pCmdLine)
{
    char **args;
    args = pCmdLine->arguments;
    char *command = args[0];
    char *path = args[1];
    if (strcmp(command, "procs") == 0)
    {
        printProcessList(&process_list);
        return 1;
    }

    return 0;
}

shouldExit(char *str)
{
    if (strcmp(str, "quit") == 0)
    {
        printf("Exit Program\n");
        freeProcessList(&process_list);
        exit(0);
    }
}

//task2
void addProcess(process **process_list, cmdLine *cmd, __pid_t pid);
char *getCommand(char **command, int size);
void printProcess(process *process, int index);
void printProcessList(process **process_list);

void addProcess(process **process_list, cmdLine *cmd, __pid_t pid)
{
    process *newProcess = (process *)malloc(sizeof(process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    newProcess->status = RUNNING;
    newProcess->next = NULL;

    if (*process_list == NULL)
    {
        *process_list = newProcess;
    }
    else
    {
        process *temp = *process_list;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newProcess;
        temp = NULL;
    }
}

char *getCommand(char *arguments[], int size)
{
    int i = 0;
    char *fullComand = calloc(1, sizeof(arguments));
    while (i < size)
    {
        strcat(fullComand, arguments[i]);
        strcat(fullComand, ",");
        i++;
    }
    return fullComand;
}

void printProcess(process *process, int index)
{
    printf("Index of process is: %d\t", index);
    printf("Process id: %d\t", process->pid);
    printf("Process status: %d\t", process->status);
    char *command = getCommand(process->cmd->arguments, process->cmd->argCount);
    printf("The command with args is: %s\n", command);
    free(command);
}

void printProcessList(process **process_list)
{
    updateProcessList(process_list);
    int i = 0;
    process *temp = *process_list;
    while (temp != NULL)
    {
        printProcess(temp, i);
        i++;
        temp = temp->next;
    }
    updateProcessList(process_list);
}

void freeProcessList(process **process_list)
{
    process *list = *process_list;
    process *temp = NULL;

    while (list != NULL)
    {
        temp = list;
        list = list->next;
        freeCmdLines(temp->cmd);
        temp->next = NULL;
        free(temp);
    }
    temp = NULL;
    list = NULL;
}

void updateProcessList(process **process_list)
{
    int info;
    int id;
    process *list = *process_list;
    if (list == NULL)
    {
        printf("The list is empty\n");
        return;
    }

    labelWhile:
    while (list != NULL)
    {
        if (list->status == TERMINATED)
        {
            deleteList(process_list, list->pid);
            list = list->next;
            goto labelWhile;
        }
        id = waitpid(list->pid, &info, WNOHANG | WUNTRACED | WCONTINUED);
        printf("waitpid id is: %d\n", id);
        if (id == -1)
        {
            updateProcessStatus(process_list, list->pid, TERMINATED);
            perror("Not pid/proccess finish");
        }
        else if (id == 0)
        {

            printf("The child %d still runing\n", list->pid);
        }

        else if (id == list->pid)
        {
            if (WIFEXITED(info))
            {
                updateProcessStatus(process_list, id, TERMINATED);
            }
            else if (WIFSTOPPED(info))
            {
                printf("child process stopped\n");
                updateProcessStatus(process_list, id, SUSPENDED);
            }
            else if (WIFCONTINUED(info))
            {
                printf("child process resumed\n");
                updateProcessStatus(process_list, id, RUNNING);
            }
            else if (WIFSIGNALED(info))
            {
                int singal = WTERMSIG(info);
                printf("procces terminated by signal %d\n", singal);
                updateProcessStatus(process_list, id, TERMINATED);
            }
        }

        list = list->next;
    }
}

void updateProcessStatus(process **process_list, int pid, int status)
{
    process *list = *process_list;
    while (list != NULL)
    {
        if (list->pid == pid)
        {
            list->status = status;
            break;
        }
        list = list->next;
    }
}

void deleteList(process **process_list, int pid)
{
    process *list = *process_list;
    while (list != NULL)
    {
        if (list->pid == pid) 
        {
            process *temp = list;
            *process_list = list->next;
            freeCmdLines(temp->cmd);
            free(temp);
            break;
        }

        else if (list->next->pid == pid)
        {

            process *temp = list->next;
            list->next = list->next->next;
            freeCmdLines(temp->cmd);
            free(temp);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int check = 0;
    bugMode = isBugMode(argc, argv);
    char input[MAX_LINE];
label:
    while (1)
    {
        printDirectory();
        fgets(input, MAX_LINE, stdin);
        input[strlen(input) - 1] = 0;
        shouldExit(input);
        pCmdLine = parseCmdLines(input);
        blockMode = pCmdLine->blocking;
        if (strcmp(pCmdLine->arguments[0], "procs") == 0 && pid != 0)
        {
            printProcessList(&process_list);
            freeCmdLines(pCmdLine);
            goto label;
        }
        else if (strcmp(pCmdLine->arguments[0], "kill") == 0)
        {
            kill(atoi(pCmdLine->arguments[1]), SIGINT);
            updateProcessStatus(process_list, atoi(pCmdLine->arguments[1]), TERMINATED);
            freeCmdLines(pCmdLine);
            goto label;
        }
        else if (strcmp(pCmdLine->arguments[0], "suspend") == 0)
        {
            kill(atoi(pCmdLine->arguments[1]), SIGTSTP);
            updateProcessStatus(process_list, atoi(pCmdLine->arguments[1]), SUSPENDED);
            sleep(pCmdLine->arguments[1]);
            kill(atoi(pCmdLine->arguments[1]), SIGCONT);
            freeCmdLines(pCmdLine);
            goto label;
        }

        int check = checkIfNeedToCd(pCmdLine);
        if (check)
        {
            freeCmdLines(pCmdLine);
            goto label;
        }

        if ((pid = fork()))
        {
            addProcess(&process_list, pCmdLine, pid);
            checkIfNeedToWait(pid);
            printPid(pid);
        }

        else
        {
            sleep(1);
            printPid(pid);
            execute(pCmdLine);
            perror("Error: ");
            exit(1);
        }
    }
}