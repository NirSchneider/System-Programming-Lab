#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char debug_mode; // 0 - off, 1 - on
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    char display_mode; // 0 - decimal, 1 - hexadecimal
} state;

struct fun_desc {
    char *name;
    void (*fun)(state*);
};

void toggleDebugMode(state* s);
void setFileName(state* s);
void setUnitSize(state* s);
void loadIntoMemory(state* s);
void toggleDisplayMode(state* s);
void memoryDisplay(state* s);
void saveIntoFile(state* s);
void memoryModify(state* s);
void quit(state* s);

struct fun_desc menu[] ={
    {"Toggle Debug Mode",toggleDebugMode},
    {"Set File Name",setFileName},
    {"Set Unit Size",setUnitSize},
    {"Load Into Memory",loadIntoMemory},
    {"Toggle Display Mode",toggleDisplayMode},
    {"Memory Display",memoryDisplay},
    {"Save Into File",saveIntoFile},
    {"Memory Modify",memoryModify},
    {"Quit",quit},
    {NULL,NULL}
};

state* initState(void);

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
    printf("Enter new unit size: ");
    scanf("%d",&newUnit);
    if(newUnit == 1 || newUnit == 2 || newUnit == 4){
        s->unit_size = newUnit;
        if(s->debug_mode)
            fprintf(stderr,"Debug: set size to %d\n",s->unit_size);
   
    }
    else{
        fprintf(stderr,"Error: %d is illegal, use 1, 2 or 4 only\n",newUnit);
    }
}

void quit(state* s){
    if(s->debug_mode)
        fprintf(stderr,"quitting\n");
    exit(0);
}

void loadIntoMemory(state* s){
    FILE* f = NULL;
    int location,length;
    if(strcmp(s->file_name,"") == 0){
        fprintf(stderr,"Error: Load into memory - Can not load when file name NULL\n");
        return;
    }
    if(!(f = fopen(s->file_name,"r"))){
        fprintf(stderr,"Error: Load into memory - Open file failed.\n");
        return;
    }
    puts("Please enter <location> <length>");
    scanf("%x %d",&location,&length);

    if(fseek(f,location,SEEK_SET) == -1){
        fprintf(stderr,"Error: Load into memory - Seek file failed.\n");
        return;
    }
    if(fread(s->mem_buf,s->unit_size,length,f) < length){
        fprintf(stderr,"Error: Load into memory - Read file failed.\n");
        return;
    }

    printf("Loaded %d units into memory\n",length);
    s->mem_count += (length*(s->unit_size));
    fclose(f);
}

void toggleDisplayMode(state* s){
    if(s->display_mode){
        s->display_mode = 0;
        printf("Display flag now off, decimal representation\n");
    }
    else{
        s->display_mode = 1;
        printf("Display flag now on, hexadecimal representation\n");
    }
}

void memoryDisplay(state* s){
    int u,addr,i;
    char* format;
    unsigned char* p_char;
    unsigned short* p_short;
    unsigned int* p_int;

    puts("Please enter <units> <adderss>");
    scanf("%d %x",&u,&addr);
    addr = addr == 0 ? (unsigned int) s->mem_buf : addr;
    if(s->display_mode){
        format = "%x\n";
        puts("Hexadecimal\n===========");
    }
    else{
        format = "%d\n";
        puts("Decimal\n=======");
    }
    
    switch (s->unit_size){
    case 1:
        p_char = (unsigned char*) addr;
        for(i = 0; i < u ;i++){
            printf(format,*(p_char+i));
        }
        break;
    case 2:
        p_short = (unsigned short*)addr;
        for(i = 0; i < u ; i++){
            printf(format,*(p_short+i));
        }
        break;
    case 4:
        p_int = (unsigned int*)addr;
        for(i = 0; i < u ;i++){
            printf(format,*(p_int+i));
        }
        break;
    default:
        fprintf(stderr,"Error: unit_size is wrong\n");
        break;
    }
}

void saveIntoFile(state* s){
    int srcAdder,targetLoc,len;
    FILE* f;
    char* p_char;
    short* p_short;
    int* p_int;

    puts("Please enter <source-address> <target-location> <length>");
    scanf("%x %x %d",&srcAdder,&targetLoc,&len);
    srcAdder = srcAdder == 0 ? (int) s->mem_buf : srcAdder;
    
    if(strcmp(s->file_name,"") == 0){
        fprintf(stderr,"Error: Save Into File - Can not save when file name NULL\n");
        return;
    }
    if(!(f = fopen(s->file_name,"r+"))){
        fprintf(stderr,"Error: Save Into File - Open traget file failed.\n");
        return;
    }
    if(fseek(f,targetLoc,SEEK_SET) == -1){
        fprintf(stderr,"Error: Save Into File - Seek target file failed\n");
        return;
    }

    switch(s->unit_size){
        case 1:
            p_char = (char*) srcAdder;
            if((fwrite(p_char,s->unit_size,len,f)) < len){
                fprintf(stderr,"Error: Save Into File - Writing to file\n");
            }
            break;
        case 2:
            p_short = (short*) srcAdder;
            if((fwrite(p_short,s->unit_size,len,f)) < len){
                fprintf(stderr,"Error: Save Into File - Writing to file\n");
            }
            break;
        case 4:
            p_int = (int*) srcAdder;
            if((fwrite(p_int,s->unit_size,len,f)) < len){
                fprintf(stderr,"Error: Save Into File - Writing to file\n");
            }
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
    if(s->debug_mode){
        fprintf(stderr,"Debug: Memory modify - location = %x, val = %x\n",loc,val);
    }

    if(loc > 10000-(s->unit_size)){
        fprintf(stderr,"Error: Memory Modify - location is out of bound\n");
        return;
    }
    memcpy((s->mem_buf)+loc,p_val,s->unit_size);
}

state* initState(void){
    state* s  = (state*)malloc(sizeof(state));
    strncpy(s->file_name,"",sizeof(s->file_name));
    s->debug_mode = 0;
    s->unit_size = 1;
    s->display_mode = 0;
    s->mem_count = 0;
    return s;
}