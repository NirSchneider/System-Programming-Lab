#include "util.h"

#define CREATE 64
#define ReadMode 0
#define WriteAndRead 2
#define EXIT 1
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define READ 3
#define WRITE 4
#define OPEN 5
#define CLOSE 6
#define SEEK_SET 0
#define SEEK_END 2
#define ERROR -1

int bugMode = 0;

void printBug(int Id, int returnValue)
{
    char * callId = "The system call ID is: ";
    char * value = "The system call return code is: ";
    char *nextLine = "\n";

    system_call(WRITE, STDERR, callId, strlen(callId));
    system_call(WRITE, STDERR, itoa(Id), strlen(itoa(Id)));
    system_call(WRITE, STDERR, nextLine, strlen(nextLine));
    system_call(WRITE, STDERR, value, strlen(value));
    system_call(WRITE, STDERR, itoa(returnValue), strlen(itoa(returnValue)));
    system_call(WRITE, STDERR, nextLine, strlen(nextLine));
}

void printPath(char *path)
{
    char *input = "The  path is: ";
    char *nextLine = "\n";

    system_call(WRITE, STDERR, input, strlen(input));
    system_call(WRITE, STDERR, path, strlen(path));
    system_call(WRITE, STDERR, nextLine, strlen(nextLine));
}
char upToLowcase(char c)
{
    int maxUpChar = 'Z' + 1;
    int minUpchar = 'A' - 1;
    int subUpLow = 'z' - 'Z';
    if (c > minUpchar && c < maxUpChar)
    {
        c = c + subUpLow;
    }

    return c;
}

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
int findInput(int argc, char **argv)
{
    if (argc < 2)
    {
        return 0;
    }

    int i = 1;
    for (i; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == 'i')
        {
            return i;
        }
    }

    return 0;
}

int findOut(int argc, char **argv)
{
    if (argc < 2)
    {
        return 0;
    }

    int i = 1;
    for (i; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == 'o')
        {
            return i;
        }
    }

    return 0;
}

int readOrWrite(int Id, int fd,int inn, char *buffer, int count)
{
    
    int ans = system_call(Id, fd, buffer, count);
    
    return ans;
}
int open(int Id, char *path, int access, int filePermissions)
{
    int returnValue = system_call(Id, path, access, filePermissions);
    if (returnValue <= ERROR)
    {
        char *erorr = "Can't open this File\n";
        system_call(WRITE, STDERR, erorr, strlen(erorr));
        system_call(EXIT, 0x55);
    }
}

int main(int argc, char *argv[], char *envp[])
{
    bugMode = isBugMode(argc, argv);
    int input;
    int output;
    int CheckInput = findInput(argc, argv);
    int checkOut = findOut(argc, argv);

    if (CheckInput != 0)
    {
        char *InputFile = argv[CheckInput] + 2;
        input = open(OPEN, InputFile, ReadMode, 0x644);
        if (bugMode)
        {
            printPath(argv[checkOut] + 2);
            printBug(OPEN, output);
        }
    }
    else
    {
        input = STDIN;
        if (bugMode)
        {
            printPath("stdin");
        }
    }

    if (checkOut != 0)
    {
        output = open(OPEN, argv[checkOut] + 2, CREATE | WriteAndRead  , 0x777);

        if (bugMode)
        {
            printPath(argv[checkOut] + 2);
            printBug(OPEN, output);
        }
    }
    else
    {
        output = STDOUT;
        if (bugMode)
        {
            printPath("stdout");
        }
    }

    char c[1] = {'\0'};
    int returnReadValue;
    int returnWriteValue;
    while (1)
    {
        returnReadValue = readOrWrite(READ, input,output, c, 1);
        if (returnReadValue <= 0)
        {
            break;
        }
        c[0] = upToLowcase(c[0]);
        if(c[0]!='\n'){
            returnWriteValue = readOrWrite(WRITE, output,input, c, 1);
            if (bugMode)
            {
                printBug(READ, returnReadValue);
                printBug(WRITE, returnWriteValue);
            }
        }
    }

    system_call(CLOSE, input);
    system_call(CLOSE, output);

    return 0;
}
