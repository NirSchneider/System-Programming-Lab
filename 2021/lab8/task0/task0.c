#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>


typedef struct {
    char debug_mode; // 0 - off, 1 - on
    int currentfd;
    void* map_start;
    struct stat currentfd_stat;    
} state;

struct fun_desc {
    char *name;
    void (*fun)(state*);
};

void notImplemented(state* s){
    printf("not implemented yet\n");
}

void toggleDebugMode(state* s);
void examineELFFile(state* s);
void quit(state* s);

struct fun_desc menu[] ={
    {"Toggle Debug Mode",toggleDebugMode},
    {"Examine ELF File",examineELFFile},
    {"Print Section Names",notImplemented},
    {"Print Symbols",notImplemented},
    {"Relocation Tables",notImplemented},
    {"Quit",quit},
    {NULL,NULL}
};

state* initState(void){
    state* s = (state*)malloc(sizeof(state));
    s->debug_mode = 0;
    s->currentfd = -1;
    s->map_start = NULL;
    return s;
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

void examineELFFile(state* s){
    char file_name[1000];
    printf("Enter an ELF file name: ");
    scanf("%s",file_name);
    
    //closing old currentfd
    if(s->currentfd > -1)
        close(s->currentfd);

    if(s->map_start)
        munmap(s->map_start,s->currentfd_stat.st_size); 

    if((s->currentfd = open(file_name,O_RDWR)) == -1){
        perror("error in open");
        return;
    }

    fstat(s->currentfd, &s->currentfd_stat);

    if((s->map_start = mmap(0, s->currentfd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, s->currentfd, 0)) == MAP_FAILED){
        perror("mmap failed");
        close(s->currentfd);
        s->currentfd = -1;
        return;
    }
    fprintf(stderr,s->map_start);

    Elf32_Ehdr* header = (Elf32_Ehdr*) s->map_start;

    if(strncmp((char*)header->e_ident+1,"ELF",3) != 0){
        fprintf(stderr,"Error: File is not ELF file!\n");
        close(s->currentfd);
        s->currentfd = -1;
        munmap(s->map_start,s->currentfd_stat.st_size);
        s->map_start = NULL;
        return;
    }

    printf("%c%c%c\n",header->e_ident[1],header->e_ident[2],header->e_ident[3]); // Bytes 1,2,3 of the magic number - ELF 
    switch(header->e_ident[EI_DATA]){ //The data encoding scheme of the object file
        case 1: 
            printf("Data encoded scheme of object file: 2's complement, little endian\n"); 
            break;
        case 2:
            printf("Data encoded scheme of object file: 2's complement, big endian\n");
            break;
    }
    printf("Entry point: %x\n",header->e_entry); //Entry point (hexadecimal address)
    printf("File offset in which the section header table resides: %d\n",header->e_shoff);  
    printf("Number of section header entries: %d\n",header->e_shnum);
    printf("Size of each section header entry: %d\n",header->e_shentsize);
    printf("File offset in which the program header table resides: %d\n",header->e_phoff);
    printf("Number of Program header enteries: %d\n",header->e_phnum);
    printf("Size of each section header entry: %d\n",header->e_phentsize);
    printf("\n");
}

void quit(state* s){
    if(s->currentfd > -1)
        close(s->currentfd);
    if(s->map_start)
        munmap(s->map_start,s->currentfd_stat.st_size);
    exit(0);
}

int main(int argc,char** argv){
    int i,choosen;
    unsigned int menuSize = sizeof menu / sizeof menu[0]-1;
    state* s = initState();
    while(1){
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
