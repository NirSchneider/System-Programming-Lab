#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h" //change it to .h when use makefile
#include <fcntl.h>
#define MAX_LINE 2048
#define MAX_NUM_HISTORY 10
//globals
int bugMode = 0;
int num_in_history =0;
__pid_t pid;
cmdLine *pCmdLine = NULL;
int blockMode = 0;
void printCommand(char *str);
int waitpid();


int isBugModeOn(int argc, char **argv)
{
    char *bugMode = "-d";
    
    for (int i = 1; i < argc; i++)
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
    //char **args = pCmdLine->arguments;
    //char *command = pCmdLine->arguments[0];
    if (strcmp(pCmdLine->arguments[0], "cd") == 0)
    {
        exit(1); //The parent do it.
    }
    printCommand(pCmdLine->arguments[0]);
    int fdOutput = open(pCmdLine->outputRedirect,O_CREAT | O_RDWR, 0600);
    int fdInput = open(pCmdLine->inputRedirect, O_RDWR, 0600);
    dup2(fdOutput, STDOUT_FILENO);
    dup2(fdInput, STDIN_FILENO);
    close(fdInput);
    close(fdOutput);
    execvp(pCmdLine->arguments[0], pCmdLine->arguments);
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
    //char **args= pCmdLine->arguments;
     
    char *command = pCmdLine->arguments[0];
    char *path = pCmdLine->arguments[1];
    if (strcmp(command, "cd") == 0)
    {
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

void shouldExit(char *str,char ** myHistory)
{
    if (strcmp(str, "quit") == 0)
    {
        for(int i=0;i<num_in_history;i++){
            free(myHistory[i]);
        }
        free(myHistory);
        printf("Exit Program\n");
        
        exit(1);
    }
}

void addToHistory(char ** myHistory , char * cmd11){
    
    if(num_in_history<10){
     num_in_history=(num_in_history+1);
    }
    
        for (int i =MAX_NUM_HISTORY-1 ; i>0 ; i-- ) {
            //free(myHistory[i]);
            myHistory[i]=myHistory[i-1];
        }
        //free(myHistory[0]);
        myHistory[0]=cmd11;

    
    
}

void printHistory(char ** myHistory[] ){
    
    for(int i=num_in_history-1;i>=0;i--){
        printf("%d\t %s\n",(num_in_history-i)-1,myHistory[i]);
    }
    //addToHistory(myHistory,"history");
}



int main(int argc, char *argv[])
{
    char ** myHistory[MAX_NUM_HISTORY]; // maybe init with null  
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
        //shouldExit(input,myHistory);
        
        if(strncmp(input,"!",1)==0){
            int temp;
            
            sscanf(input,"%*[^0123456789]%d",&temp);


            if(temp>9){
                fprintf(stderr,"ERR :out of bound");
                goto startWhile;
            }
            

            strcpy(input,myHistory[num_in_history-temp-1]); 
        }

        char * input1=strdup(input);
        //if(!(strcmp(input,"history")==0)){
            addToHistory(myHistory,input1);
        //}
        pCmdLine = parseCmdLines(input);

        if (strcmp(pCmdLine->arguments[0], "quit") == 0)
        {
            for(int i=0;i<num_in_history;i++){
                free(myHistory[i]);
            }
            //free(myHistory); 
            freeCmdLines(pCmdLine);
            printf("Exit Program\n");
            exit(1);
        }
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

        
        if(strcmp(pCmdLine->arguments[0],"history")==0){
            printHistory(myHistory);
            freeCmdLines(pCmdLine);
            goto startWhile;
        }

        
        if (!(pid = fork())) //child
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