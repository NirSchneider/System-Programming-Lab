#include "util.h"
#define EXIT 1
#define STDOUT 1
#define STDERR 2
#define OPEN 5
#define WRITE 4
#define CLOSE 6
#define BUF_SIZE 8192
#define GETDENTS 141

enum
{
    DT_UNKNOWN = 0,
#define DT_UNKNOWN DT_UNKNOWN
    DT_FIFO = 1,
#define DT_FIFO DT_FIFO
    DT_CHR = 2,
#define DT_CHR DT_CHR
    DT_DIR = 4,
#define DT_DIR DT_DIR
    DT_BLK = 6,
#define DT_BLK DT_BLK
    DT_REG = 8,
#define DT_REG DT_REG
    DT_LNK = 10,
#define DT_LNK DT_LNK
    DT_SOCK = 12,
#define DT_SOCK DT_SOCK
    DT_WHT = 14
#define DT_WHT DT_WHT
};

int bugmode = 0;
int prefixmod = 0;

typedef struct myStruc
{
    int node;
    int offset;
    short length;
    char name[1];
} myStruc;


char *findType(char c)
{
    switch (c)
    {
        case DT_BLK:
            return "block device";
        case DT_CHR:
            return "character device";
        case DT_DIR:
            return "directory";
        case DT_FIFO:
            return "pipe";
        case DT_LNK:
            return "symbol link";
        case DT_REG:
            return "regular file";
        case DT_SOCK:
            return "domain socket";
        case DT_UNKNOWN:
            return "unknow";
        default:
            return "";
    }
}

void printBug(int Id, int returnValue)
{
    char *callId = "The system call ID is: ";
    char *value = "The system call value is: ";
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
    system_call(WRITE, STDERR, path, strlen(input));
    system_call(WRITE, STDERR, nextLine, strlen(nextLine));
}

void printArgsSystemCall(int id, int fd, char *str, int size)
{
    char *theArgs = "the args are: ";
    char *nextLine = "\n";
    char *tab = "\t";

    system_call(WRITE, STDERR, theArgs, strlen(theArgs));
    system_call(WRITE, STDERR, itoa(id), strlen(itoa(id)));
    system_call(WRITE, STDERR, tab, strlen(tab));
    system_call(WRITE, STDERR, itoa(fd), strlen(itoa(fd)));
    system_call(WRITE, STDERR, tab, strlen(tab));
    system_call(WRITE, STDERR, str, strlen(str));
    system_call(WRITE, STDERR, tab, strlen(tab));
    system_call(WRITE, STDERR, itoa(size), strlen(itoa(size)));
    system_call(WRITE, STDERR, nextLine, strlen(nextLine));
}

void printLengthNameDirent(int length, char *name)
{
    char *Thelength = "The length of the record is: ";
    char *nameOfRecord = "The name of the record is: ";
    char *nextLine = "\n";

    system_call(WRITE, STDERR, Thelength, strlen(Thelength));
    system_call(WRITE, STDERR, itoa(length), strlen(itoa(length)));
    system_call(WRITE, STDERR, nextLine, strlen(nextLine));
    system_call(WRITE, STDERR, nameOfRecord, strlen(nameOfRecord));
    system_call(WRITE, STDERR, name, strlen(name));
    system_call(WRITE, STDERR, nextLine, strlen(nextLine));
}

void printPrefix(char *fileName, char *prefix, char type)
{
    char *strType = findType(type);
    if (strncmp(fileName, prefix, strlen(prefix)) == 0)
    {
        ReadOrWriteOrGetdents(WRITE, STDOUT, fileName, strlen(fileName));
    
    char *theType = "The type file is: ";

    system_call(WRITE, STDOUT, theType, strlen(theType));
    ReadOrWriteOrGetdents(WRITE, STDOUT, strType, strlen(strType));
    }
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
int FindPrefix(int argc, char **argv)
{
    char *prefix = "-p";
    int i = 1;
    for (i; i < argc; i++)
    {
        if (strncmp(argv[i], prefix, strlen(prefix)) == 0)
        {
            return 1;
        }
    }
    return 0;
}
int ReadOrWriteOrGetdents(int Id, int filedesc, char *buffer, int count)
{
    char *nextLine = "\n";
    int returnValue = system_call(Id, filedesc, buffer, count);
    if (bugmode)
    {
        printBug(Id, returnValue);
        printArgsSystemCall(Id, filedesc, buffer, count);
    }

    if (Id == WRITE)
    {
        system_call(WRITE, STDOUT, nextLine, strlen(nextLine));
    }

    return returnValue;
}


int main(int argc, char *argv[], char *envp[])
{
    char type;
    char *prefix;
    bugmode = isBugMode(argc, argv);
    prefixmod = FindPrefix(argc, argv);
    if (prefixmod)
    {
        prefix = argv[prefixmod] + 2;
    }

    char buf[8192];
    myStruc *ent = buf;
    int filedesc = system_call(OPEN, ".", 0, 0);
    if (bugmode)
    {
        char *comment = "print the path";
    }
    
    int nread = ReadOrWriteOrGetdents(GETDENTS, filedesc, buf, 8192);
    if (nread < 0)
    {
        ReadOrWriteOrGetdents(WRITE, STDERR, "Error in Getdents\n", strlen("Error in Getdents\n"));
        system_call(EXIT, 0x55);
    }
    
    char *name;
    int i = 0;
    while (i < nread)
    {
        ent = (buf + i);
        int length = ent->length;
        name = ent->name;
        int sizeOfName = length - 2 * (sizeof(int)) - sizeof(short);
        type = *(buf + i + ent->length - 1);
        if (prefixmod)
        {
            printPrefix(name, prefix, type);
        }
        else
        {
            ReadOrWriteOrGetdents(WRITE, STDOUT, name, sizeOfName);
        }

        i += ent->length;
    }
    
    system_call(CLOSE, filedesc);
}