#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  NAME_LEN  128
#define  BUF_SZ    10000

int u,addr,i;

typedef struct {
  char debug_mode;
  char file_name[NAME_LEN];
  int unit_size;
  unsigned char mem_buf[BUF_SZ];
  size_t mem_count;
  /*
   .
   .
   Any additional fields you deem necessary
  */
} state;

struct fun_desc {
  char *name;
  void (*fun)(state*);
};


void toggleDebugMode(state* s){
    if(s->debug_mode){
        s->debug_mode = 0;
        printf("Debug flag now off\n");
    }
    else{
        s->debug_mode = 1;
        printf("Debug flag now on\n");
    }
}


void setFileName(state* s){
    printf("Enter new file name: ");
    scanf("%s",s->file_name);
    if(s->debug_mode)
        fprintf(stderr,"Debug: file name set to '%s'\n",s->file_name);
    
}


void setUnitSize(state* s){
    int newUnit;
    printf("Enter new unit size: "); //1. Prompt the user for a number
    scanf("%d",&newUnit);
    if(newUnit == 1 || newUnit == 2 || newUnit == 4){ //2. If the value is valid (1, 2, or 4), set the size variable accordingly
        s->unit_size = newUnit;
        if(s->debug_mode) //3.  debug mode is on, print "Debug: set size to x"
            fprintf(stderr,"Debug: set size to %d\n",s->unit_size);
   
    }
    else{ //4. If not valid, print an error message and leave size unchanged
        fprintf(stderr,"Error: %d is illegal, use 1, 2 or 4 only\n",newUnit);
    }
}


void loadIntoMemory(state* s){
    FILE* f = NULL;
    int location,length;
    if(strcmp(s->file_name,"") == 0){ //Check if file_name is empty (i.e. equals to ""), and if it is print an error message and return.
        fprintf(stderr,"Error: Load into memory - Can not load when file name NULL\n");
        return;
    }
    if(!(f = fopen(s->file_name,"r"))){ // Open file_name for reading. If this fails, print an error message and return.
        fprintf(stderr,"Error: Load into memory - Open file failed.\n");
        return;
    }
    puts("Please enter <location> <length>"); //Prompt the user for location (in hexadecimal) and length (in decimal).
    scanf("%x %d",&location,&length);

    if (s->debug_mode){ //If debug flag is on, print the file_name, as well as location, and length
        fprintf(stderr, "Debugging: File name:%s\nlocation:%d\nlength:%d\n",s->file_name,location,length);
    }
    if(fseek(f,location,SEEK_SET) == -1){
        fprintf(stderr,"Error: Load into memory - Seek file failed.\n");
        return;
    }
 
    //Copy length * unit_size bytes from file_name starting at position location into mem_buf.
    if(fread(s->mem_buf,s->unit_size,length,f) < length){ 
        fprintf(stderr,"Error: Load into memory - Read file failed.\n");
        return;
    }

    printf("Loaded %d units into memory\n",length);
    s->mem_count += (length*(s->unit_size));
    fclose(f); //Close the file. 
}

void quit(state* s){
    if(s->debug_mode)
        fprintf(stderr,"quitting\n");
    exit(0);
}

void printBySize(int unitSize, char* format){
    unsigned char* p_char;
    unsigned short* p_short;
    unsigned int* p_int;
    switch (unitSize){
    case 1:
        p_char = (unsigned char*) addr;
        for(i = 0; i < u ;i++){
            printf(format,*(p_char+i));
        }
        printf("\n");
        break;
    case 2:
        p_short = (unsigned short*)addr;
        for(i = 0; i < u ; i++){
            printf(format,*(p_short+i));
        }
        printf("\n");
        break;
    case 4:
        p_int = (unsigned int*)addr;
        for(i = 0; i < u ;i++){
            printf(format,*(p_int+i));
        }
        printf("\n");
        break;
    default:
        fprintf(stderr,"Error: unit_size is wrong\n");
        break;
    }

}

void memoryDisplay(state* s){

    char* format;
    
    puts("Enter address and length");
    scanf("%x %d",&addr,&u);

    addr = addr == 0 ? (unsigned int) s->mem_buf : addr;
    format = "%x\n";
    puts("Hexadecimal\n===========");
    printBySize(s->unit_size, format);
    format = "%d\n";
    puts("Decimal\n=======");
    printBySize(s->unit_size, format);

    

}

void printNothing(state* s){
    if(s->debug_mode){
        printf("Debugging: not implemented yet\n");
    }
    else{
        printf("not implemented yet\n");
    }
}

state* initState(void){
    state* s  = (state*)malloc(sizeof(state));
    strncpy(s->file_name,"",sizeof(s->file_name));
    s->debug_mode = 0;
    s->unit_size = 1;
    s->mem_count = 0;
    return s;
}


struct fun_desc menu[] ={
    {"Toggle Debug Mode",toggleDebugMode},
    {"Set File Name",setFileName},
    {"Set Unit Size",setUnitSize},
    {"Load Into Memory",loadIntoMemory},
    {"Memory Display", memoryDisplay},
    {"Save Into File", printNothing},
    {"Memory Modify", printNothing},
    {"Quit",quit},
    {NULL,NULL}
};

int main(int argc,char** argv){
    int i,choosen;
    unsigned int menuSize = sizeof menu / sizeof menu[0]-1;
    state* s = initState();
    while(1){
        if(s->debug_mode)
            fprintf(stderr,"Unit size:%d\nFile name:%s\nMem count:%d\n",s->unit_size,s->file_name,s->mem_count);
        //print menu
        for(i = 0 ; i<menuSize;i++){
            printf("%d-%s\n",i,menu[i].name);
        }
        scanf("%d",&choosen);
        if(choosen >= 0 && choosen <menuSize)
            menu[choosen].fun(s);
        else
            fprintf(stdout,"Error: %d is an illegal choice\n",choosen);
        

    }
    free(s);
    return 0;
}

