#include <stdio.h>
#include <stdlib.h>

void PrintHex(char buffer[], int length)
{
    for(int i=0; i<length; i++)
        printf("%x ", (unsigned char) buffer[i]);
    printf("\n");
}

int main(int argc, char **argv) {
    FILE* input = fopen(argv[1], "r");
    char c;
    int counter = 0;
    while((c=fgetc(input)) != EOF)
        counter++;
    fseek(input, 0, SEEK_SET );
    char buffer[counter];
    fread(buffer, 1, counter, input);
    PrintHex(buffer, counter);
    return 0;
}