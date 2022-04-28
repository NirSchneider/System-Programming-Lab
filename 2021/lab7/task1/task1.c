#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  NAME_LEN  128
#define  BUF_SZ    10000

typedef struct {
  char debug_mode;
  char file_name[NAME_LEN];
  int unit_size;
  unsigned char mem_buf[BUF_SZ];
  size_t mem_count;
} state;

struct fun_desc {
  char *name;
  void (*fun)(state*);
};

int u,addr,i;

void toggleDebugMode(state* s){
    if(s->debug_mode){
        s->debug_mode = 0;
        printf("Debug off\n");
    }
    else{
        s->debug_mode = 1;
        printf("Debug on\n");
    }
}

void setFileName(state* s){
    printf("Enter file name:");
    scanf("%d",s->file_name);
    if(s->debug_mode)
        fprintf(stderr,"Debug: the file is '%d'\n", s->file_name);
}


void setUnitSize(state* s){
    int nu;
    printf("Enter new unit size: ");
    scanf("%d", &nu);
    if(nu == 1 || nu == 2 || nu == 4)
    { 
        s->unit_size = nu;
    }
    else
    {
        fprintf(stderr,"Error: %s 1, 2 or 4 only\n",nu);
    }
}

void loadIntoMemory(state* s){
    FILE* f = NULL;
    int location,length;
    printf("%d units loaded\n",length);
    s->mem_count += (length*(s->unit_size));
    fclose(f); //Close the file. 
}

void quit(state* s){
    if(s->debug_mode)
        fprintf(stderr,"quitting\n");
    exit(0);
}

void printBySize(int unitSize, char* format){

}

void memoryDisplay(state* s){

}

void saveIntoFile(state* s){
    int srcAdder,targetLoc,len;
    FILE* f;
    char* i_char;
    short* i_short;
    int* i_int;

    puts("Please enter <source-address> <target-location> <length>");
    scanf("%x %x %d",&srcAdder,&targetLoc,&len);
    srcAdder = srcAdder == 0 ? (int) s->mem_buf : srcAdder;

    switch(s->unit_size){
        case 1:
            i_char = (char*) srcAdder;
            fwrite(i_char,s->unit_size,len,f);
            break;
        case 2:
            i_short = (short*) srcAdder;
            fwrite(i_short,s->unit_size,len,f);
            break;
        case 4:
            i_int = (int*) srcAdder;
            fwrite(i_int,s->unit_size,len,f);
            break;
        default:
            fprintf(stderr,"Error: unit_size is wrong\n");
            break;
    }   

    fclose(f);
}

void memoryModify(state* s){
    int loc,val;
    puts("Please enter <location> <val>"); 
    scanf("%x %x",&loc,&val);
    char* p_val = (char*)&val;
    if(loc > BUF_SZ-(s->unit_size)){
        return;
    }
    memcpy((s->mem_buf)+loc,p_val,s->unit_size); 
}


struct fun_desc menu[] ={
    {"Toggle Debug Mode",toggleDebugMode},
    {"Set File Name",setFileName},
    {"Set Unit Size",setUnitSize},
    {"Load Into Memory",loadIntoMemory},
    {"Memory Display", memoryDisplay},
    {"Save Into File", saveIntoFile},
    {"Memory Modify", memoryModify},
    {"Quit",quit},
    {NULL,NULL}
};

int main(int argc,char** argv){
    int i,choosen;
    unsigned int menuSize = sizeof menu / sizeof menu[0]-1;
    state* s;
    while(1){
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

