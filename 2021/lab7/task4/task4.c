#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fixedCount(char* str){
    int output=0;
    for (int i = 0; str[i]; i++ ){
        if(str[i] <= '9' && str[i] >= '0')
            output++;
    }
    return output;
}

int main(int argc,char** argv){
    if(argc >1){
        int count = fixedCount(argv[1]);
        printf("The number of digits in the string is: %d\n",count);
    }
    return 0;
}
