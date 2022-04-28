#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
 char censor(char c) {
     if(c =='!'){
         return '.';
     }
     return c;
 }

char encrypt(char c) {
    if ( c< 0x7E && c> 0x20) {
        return c+3;
    }
    return c;
}

char decrypt(char c) {
    if ( c< 0x7E && c> 0x20) {
        return c-3;
    }
    return c;
}

char cprt(char c) {
    if ( c< 0x7E && c> 0x20) {
        printf("%c\n",c);
    }
    else {
        printf("%c\n",'.');
    }
    
    return c;
}

char my_get (char c) {
    char temp = getc(stdin);
    return temp;
}

char* map(char * array,int array_length,char(*f)(char)){
    char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    for(int i =0; i<array_length;i++) {
        mapped_array[i] = f(array[i]);
    }
    return mapped_array;
}
