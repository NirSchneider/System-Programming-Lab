#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <linux/limits.h>
#include <fcntl.h>

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

int main(int argc, char *argv[])
{
    int bugmode = isBugModeOn(argc, argv);
    int fd[2];
    pipe(fd);
    if (bugmode)
        fprintf(stderr, "parent_process>forking..\n");
    pid_t pid1 = fork();

    //child1
    if (!pid1)
    {
        if (bugmode)
            fprintf(stderr, "child1> redirecting stdout to the read end of the pipe...\n");
        close(STDOUT_FILENO);
        dup(fd[1]);
        close(fd[1]);
        char *args1[] = {"ls", "-l", NULL};
        if (bugmode)
            fprintf(stderr, "child1> going to execute cmd: %s\n", args1[0]);
        execvp("ls", args1);
        exit(1);
    }
    //parent
    else
    {
        if (bugmode)
            fprintf(stderr, "parent_process>create proccess with id: %d\n", pid1);
        if (bugmode)
            fprintf(stderr, "parent_process>closing the write end of the pipe\n");
        close(fd[1]); // close the write pipe
        waitpid(pid1, NULL, 0);
        pid_t pid2 = fork();

        //child2
        if (!pid2)
        {
            if (bugmode)
                fprintf(stderr, "child2> redirecting stdin to the read end of the pipe...\n");
            close(STDIN_FILENO);
            dup(fd[0]);
            close(fd[0]);
            char *args2[] = {"tail", "-n", "2", NULL};
            if (bugmode)
                fprintf(stderr, "child2> going to execute cmd: %s\n", args2[0]);
            execvp(args2[0], args2);
            exit(1);
        }
        //parent
        else
        {
            if (bugmode)
                fprintf(stderr, "parent_process>create proccess with id: %d\n", pid2);
            if (bugmode)
                fprintf(stderr, "parent_process>closing the write read of the pipe\n");
            close(fd[0]);
            if (bugmode)
                fprintf(stderr, "parent_process> waiting for child processes to terminate...\n");
              waitpid(pid2, NULL, 0);
            if (bugmode)
                fprintf(stderr, "parent_process> exiting...\n");
        }
    }
}
