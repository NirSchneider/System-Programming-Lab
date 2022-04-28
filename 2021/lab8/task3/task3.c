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
    int fd;
    void* map_start;
    struct stat fd_stat;    
} state;

struct fun_desc {
    char *name;
    void (*fun)(state*);
};

void toggleDebugMode(state* s);
void examineELFFile(state* s);
void printSectionNames(state* s);
void printSymbols(state* s);
void relocationTables(state* s);
void quit(state* s);

struct fun_desc menu[] ={
    {"Toggle Debug Mode",toggleDebugMode},
    {"Examine ELF File",examineELFFile},
    {"Print Section Names",printSectionNames},
    {"Print Symbols",printSymbols},
    {"Relocation Tables",relocationTables},
    {"Quit",quit},
    {NULL,NULL}
};

state* initState(void);

// ELF utils funcs
Elf32_Shdr* get_shdr(Elf32_Ehdr* hdr){
    return (Elf32_Shdr*)((int)hdr + hdr->e_shoff); //e_shoff- section hedear table's file offset in bytes
}

Elf32_Shdr* get_shdr_idx(Elf32_Ehdr* hdr,int idx){ //return a specific section by index
    return &get_shdr(hdr)[idx];
}

char *get_symb_name(Elf32_Ehdr *hdr,Elf32_Shdr* strtab,Elf32_Sym* symbol){
	return (char*)hdr + strtab->sh_offset + symbol->st_name;
}

Elf32_Sym *get_sybm(Elf32_Ehdr* hdr, Elf32_Shdr *shdr){
	return (Elf32_Sym*)((int)hdr + shdr->sh_offset);
}
 
Elf32_Sym *get_symb_idx(Elf32_Ehdr* hdr,Elf32_Shdr *shdr, int idx){
	return &get_sybm(hdr,shdr)[idx];
}

char* str_table(Elf32_Ehdr* hdr){ 
    if(hdr->e_shstrndx == SHN_UNDEF) //e_shstrndx-index of the entry that associated with the section name string table. SHN_UNDEF - if no string table
        return NULL;
    return (char*)hdr + get_shdr_idx(hdr,hdr->e_shstrndx)->sh_offset; //sh_offset - the byte offset from the beggining of the file to the first byte in the section.
}

char* get_shdr_name(Elf32_Ehdr* hdr, Elf32_Shdr* secHdr){ //returns the name of the specific secHdr
    char* strtab = str_table(hdr);
    //fprintf(stdout, strtab);
    if(!strtab)
        return NULL;
    return strtab + (secHdr->sh_name);
}

Elf32_Rel* get_rel(Elf32_Ehdr* hdr,Elf32_Shdr* shdr){
    return (Elf32_Rel*)((int)hdr + shdr->sh_offset);
}

Elf32_Rel* get_rel_idx(Elf32_Ehdr* hdr,Elf32_Shdr* shdr,int idx){
    return &get_rel(hdr,shdr)[idx];
}

state* initState(void){
    state* s = (state*)malloc(sizeof(state));
    s->debug_mode = 0;
    s->fd = -1;
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
    printf("Enter new ELF file name: ");
    scanf("%s",file_name);
    
    //closing old fd
    if(s->fd > -1)
        close(s->fd);

    if(s->map_start)
        munmap(s->map_start,s->fd_stat.st_size);

    if((s->fd = open(file_name,O_RDWR)) < 0){
        perror("error in open");
        return;
    }

    if(fstat(s->fd, &s->fd_stat) != 0){
        perror("stat failed");
        close(s->fd);
        s->fd = -1;
        return;
    }

    if((s->map_start = mmap(0, s->fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, s->fd, 0)) == MAP_FAILED){
        perror("mmap failed");
        close(s->fd);
        s->fd = -1;
        return;
    }

    Elf32_Ehdr* header = (Elf32_Ehdr*) s->map_start;

    if(strncmp((char*)header->e_ident+1,"ELF",3) != 0){
        fprintf(stderr,"Error: File is not ELF file!\n");
        close(s->fd);
        s->fd = -1;
        munmap(s->map_start,s->fd_stat.st_size);
        s->map_start = NULL;
        return;
    }

    printf("%c %c %c\n",header->e_ident[1],header->e_ident[2],header->e_ident[3]);
    switch(header->e_ident[EI_DATA]){
        case 1:
            printf("Data encoded scheme of object file: 2's complement, little endian\n");
            break;
        case 2:
            printf("Data encoded scheme of object file: 2's complement, big endian\n");
            break;
    }
    printf("Entry point: %x\n",header->e_entry);
    printf("Section header table offset: %d\n",header->e_shoff);
    printf("Number of section header entries: %d\n",header->e_shnum);
    printf("Size of each section header entry: %d\n",header->e_shentsize);
    printf("Program header table offset: %d\n",header->e_phoff);
    printf("Number of Program header enteries: %d\n",header->e_phnum);
    printf("Size of each section header entry: %d\n",header->e_phentsize);
    printf("\n");
}

void quit(state* s){
    if(s->fd > -1)
        close(s->fd);
    if(s->map_start)
        munmap(s->map_start,s->fd_stat.st_size);
    exit(0);
}

void printSectionNames(state* s){
    int i;
    if(!s->map_start){
        fprintf(stderr,"Error: file is not defined.\n");
        return;
    }

    Elf32_Ehdr* hdr = (Elf32_Ehdr*) s->map_start;
    unsigned int secCounter = hdr->e_shnum; //number of entries in the section header table
    Elf32_Shdr* secHdr; //file section header table
    printf("Section Headers:\n");
    printf("index\tsection_name\t\tsection_address\t\tsection_offset\t\tsection_size\t\tsection_type\n");
    if(s->debug_mode)
        printf("debug: Name offset\n");

    for(i = 0 ; i < secCounter ; i++){
        secHdr = &get_shdr(hdr)[i]; //the index of the section
        printf("[%2d]\t%-20s\t%08x\t\t%06x\t\t\t%06x\t\t\t%-8x",i,get_shdr_name(hdr,secHdr),secHdr->sh_addr,secHdr->sh_offset,secHdr->sh_size,secHdr->sh_type);
        if(s->debug_mode)
            printf("debug:  %06x",secHdr->sh_name);
        printf("\n");
    }
    printf("\n");
}

void printSymbols(state* s){
    int i,j;
    char foundSymbols = 0;

    if(!s->map_start){
        fprintf(stderr,"Error: file is not defined.\n");
        return;
    }

    Elf32_Ehdr* hdr = (Elf32_Ehdr*) s->map_start;

    unsigned int secNumber = hdr->e_shnum;
    for(i = 0 ; i < secNumber ; i++){ //SYMTAB- section that provides symbols DYNSYM- section holds minimal set of dynamic linking syms (identifies a symbol table)
        if(get_shdr_idx(hdr,i)->sh_type == SHT_SYMTAB || get_shdr_idx(hdr,i)->sh_type == SHT_DYNSYM){ //sh_type - categorizes the ection's contents
            foundSymbols = 1;
            
            Elf32_Shdr* symbSec = get_shdr_idx(hdr,i);
            int symbolsCount = symbSec->sh_size/symbSec->sh_entsize; //size- the section size in bytes. entsize- size of each element

            Elf32_Shdr* strtab = get_shdr_idx(hdr,symbSec->sh_link);//link - a section header table index link- depends on the section type
            Elf32_Sym* symbol;
            char* secName = "";
            int symbolSecIndex;
            printf("Symbol Table '%s' at offset 0x%x contains %d enteries:\n",get_shdr_name(hdr,symbSec),symbSec->sh_offset,symbolsCount);
            printf("[Nr] Value      Section index  Section name        Symbol name\n");
            for(j = 0 ; j < symbolsCount ; j++){ //print all symbols
                symbol = get_symb_idx(hdr,symbSec,j);
                symbolSecIndex = symbol->st_shndx;

                if(symbolSecIndex < secNumber){
                    secName = get_shdr_name(hdr,get_shdr_idx(hdr,symbolSecIndex)); //secName handling
                }

                printf("[%2d]  %08x  %-5d          %-20s %-20s \n",j,symbol->st_value,symbol->st_shndx,secName,get_symb_name(hdr,strtab,symbol));
                secName = "";
            }
            printf("\n");
        }
    }

    if(foundSymbols == 0){
        fprintf(stderr,"Error: no symbol table was found.\n");
    }
}

void relocationTables(state* s){
    int i,j,relSymbolIdx;

    if(!s->map_start){
        fprintf(stderr,"Error: file is not defined.\n");
        return;
    }

    Elf32_Ehdr* hdr = (Elf32_Ehdr*) s->map_start;
    unsigned int secCounter = hdr->e_shnum;
    Elf32_Shdr* symbols;
    Elf32_Sym* symEnt;

    for(i = 0 ; i < secCounter ; i++){
        if(get_shdr_idx(hdr,i)->sh_type == SHT_DYNSYM){
            if(strcmp(get_shdr_name(hdr,get_shdr_idx(hdr,i)),".dynsym") == 0){ //3b
                symbols = get_shdr_idx(hdr,i);
                break;
            }
        }
    }

    for(i = 0 ; i < secCounter ; i++){
        if(get_shdr_idx(hdr,i)->sh_type == SHT_REL){ //identifies relocation entries
            Elf32_Shdr* relSec = get_shdr_idx(hdr,i);
            int relocationCount = relSec->sh_size/relSec->sh_entsize; 

            Elf32_Shdr* strtab = get_shdr_idx(hdr,symbols->sh_link);
            Elf32_Rel* rel; //offset+info
            printf("Relocation Table '%s' at offset 0x%x contains %d enteries:\n",get_shdr_name(hdr,relSec),relSec->sh_offset,relocationCount);
            printf("[Nr] Offset     Info      Type     Sym.Value  Sym.Name \n");
            for(j = 0 ; j < relocationCount ; j++){
                rel = get_rel_idx(hdr,relSec,j);
                relSymbolIdx = ELF32_R_SYM(rel->r_info); //returns the relocation synbol index
                symEnt = get_symb_idx(hdr,symbols,relSymbolIdx);
                printf("[%2d]  %08x  %08x  %-8d %08x    %s\n",j,rel->r_offset,rel->r_info,ELF32_R_TYPE(rel->r_info),symEnt->st_value,get_symb_name(hdr,strtab,symEnt));
            }
            printf("\n");
        }
    }
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

