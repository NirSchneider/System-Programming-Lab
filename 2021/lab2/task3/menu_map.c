#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define array_size (5)
 
 char censor(char c);
 char encrypt(char c);
 char decrypt(char c);
 char crpt(char c);
 char my_get(char c);

//1:
 char temp [array_size] = {0,0,0,0,0};
 char *carray = temp;
 char *ptr = temp;

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

char crpt(char c) {
    if ( c< 0x7E && c> 0x20) {
        printf("%c\n",c);    }
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

 //2:
 struct fun_desc
 {
    char *name;
    char(*fun)(char);
 };
 struct fun_desc menu[] = {
     {"Censor",censor}, {"Encrypt",encrypt}, {"Decrypt",decrypt},
     {"Print string",crpt},{"Get string",my_get},{NULL,NULL}
 };

 //3:
 void print_menu(struct fun_desc menu[]) {
     int i = 0;
     while (menu[i].name!=NULL)
        {
            printf("%d  %s\n",i,menu[i].name);
            i++;
        }
 }
 //4:
 int findBounds(struct fun_desc menu[]) {
     int i = 0;
     while (menu[i].name!=NULL)
     {
         i++;
     }
     return i-1;
     
 }
 
 void boundCheck(int num,int bound){
     if (num<= bound && num >= 0) {
         printf("%s\n\n","Within bounds");
     }
     else {
        printf("%s\n\n","Not within bounds");
        free(carray);
        exit(0);
     }
 }

 int userChoice(struct fun_desc menu[]) {
     printf("%s\n","Please choose a function:");
     print_menu(menu);
     int num;
     scanf("%d",&num);
     return num;
 }
 
 char * copystr(char *des, char *src) {
     for (int i = 0; i < array_size; i++) {
         des[i] = src[i];
     }
     return des;
 }


int main() {
    int bound = findBounds(menu);
    while (1)
    {
        copystr(ptr,carray);
        //free(carray); //doing problems
        int num = userChoice(menu);
        boundCheck(num,bound); // if it is not in bound the program finish.
        fgetc(stdin); // to remove extra enter
        fseek(stdin,0,SEEK_END);
        carray = map(ptr,array_size,menu[num].fun);
       
    }
}