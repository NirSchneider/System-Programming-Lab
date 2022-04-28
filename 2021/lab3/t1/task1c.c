#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define lengthSigMax 128
#define bufferSize 10000
#define first18Bytes 18

int getNum() {
    char input[sizeof(int)];
    fgets(input,sizeof(int),stdin);
    int num;
    sscanf(input,"%d",&num);
    return num;
}

 unsigned int  sizeFile (FILE * file) {
    fseek(file,0L,SEEK_END);
    unsigned int length =ftell(file);
    fseek(file,0L,SEEK_SET);
    return length;
}

unsigned int minimum(int x, int y){
    if (x > y){
        return y;
    }
    return x;
}


typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

virus* readVirus(FILE* input) { 
    if (input == NULL) {
        printf("signaturs null\n");
    }
    virus * ans =(virus*) malloc(sizeof(virus));
    fread(ans,sizeof(char),first18Bytes,input); 
    char * sig = malloc(ans->SigSize);
    fread(sig,sizeof(char),ans->SigSize,input); 
    ans -> sig = sig;
    sig = NULL;
    return ans;
}

void printVirus(virus* virus, FILE* output) {
    char * virusName = "Virus name: ";
    fwrite(virusName,sizeof(char),strlen(virusName),output);
    fwrite(virus->virusName,sizeof(char),strlen(virus->virusName),output);
    fprintf(output,"%s","\n");
    char * VirusSize = "Virus Size: ";
    fwrite(VirusSize,/*1*/sizeof(char),strlen(VirusSize),output);
    fprintf(output,"%d\n",virus->SigSize);
    fprintf(output,"%s","singnature:\n");
    for (int i = 0; i < virus->SigSize;i++) {
        unsigned char hex = (unsigned char) virus->sig[i];
        fprintf(output,"%x ",hex);
    }
    fprintf(output,"%s","\n");
     }    
   

void list_print(link * virus_list,FILE* output) {
        printVirus(virus_list->vir,output);
        if(virus_list->nextVirus == NULL) {
            return;
        }
        list_print(virus_list->nextVirus,output);
}

link* list_append (link* virus_list,virus* data) { 
    if (virus_list == NULL) {
        link* new_list_ptr =(link*) malloc(sizeof(link));
        new_list_ptr->nextVirus = NULL;
        new_list_ptr->vir = data;
        return new_list_ptr;
    }
    link* newVirus = (link*) malloc(sizeof(sizeof(link)));
    newVirus->vir = data;
    newVirus->nextVirus = NULL;
    link* temp = virus_list;
    while (temp->nextVirus!=NULL)
    {
        temp = temp->nextVirus;
    }
    temp->nextVirus = newVirus;
    temp = NULL;
    return virus_list;
}


void list_free(link* virus_list) {
    link* temp = NULL;
    while(virus_list != NULL){
        temp = virus_list;
        virus_list = virus_list->nextVirus;
        free(temp->vir->sig);
        free(temp->vir);
        temp ->nextVirus = NULL;
        free(temp);
    }
    temp = NULL;
}


void detect_virus(char* buffer, unsigned int size, link* virus_list, FILE* output) {
            
    while(virus_list != NULL){
        int loc = 0;
        int sigSize = virus_list->vir->SigSize;
        while (loc + sigSize  <= size)
        {  
            if(memcmp(buffer + loc,virus_list->vir->sig,sigSize) == 0) {
                printf("Starting byte location: %d\n", loc);
                printf("Virus name: %s\n",virus_list->vir->virusName);
                printf("Sig Size is: %d\n",sigSize);
            }
            loc++;
        }
        virus_list = virus_list->nextVirus;     
    }
}


int main(int argc, char **argv) {
    FILE * signature;
    bool isLoaded = false;
    link * virusList = NULL;
    while (1)
    {
        printf("%d %s\n%d %s\n%d %s\n",1,"Load signatures",2,"Print signatures",3,"Detect viruses");
        printf("%s\n","Please choose a number");
        int num = getNum();
        if(num == 1) {
            printf("%s\n","Please put a signature file name:");
            char sigFile [lengthSigMax];
            fgets(sigFile,lengthSigMax,stdin);
            sigFile[strlen(sigFile) -1] = '\0'; // \n removal
            FILE * signature = fopen(sigFile,"r");
            if (signature == NULL){
                printf("File is null \n");
            }
            long int len = sizeFile(signature);
            fseek(signature,4,SEEK_SET);
            while(1) {
                if (ftell(signature) == len) {
                    break;
                }
                virus * a = readVirus(signature);
                virusList = list_append(virusList,a);
            }
            isLoaded = true;
            fclose(signature);
        }

        else if (num == 2) {
            if (isLoaded){
                list_print(virusList,stdout);
            }
            else {
                printf("no signatures loaded yet\n");
            }
        }

        else if (num == 3) {
            if(isLoaded) {
                char buffer[bufferSize];
                FILE* suspectFile = fopen(argv[1],"r+");
                unsigned int fileSize = sizeFile(suspectFile);
                unsigned int size = minimum(fileSize,bufferSize);
                fread(buffer,sizeof(char),size,suspectFile);
                detect_virus(buffer,size,virusList, stdout);
                fclose(suspectFile);
            }
            else {
                printf("no signatures loaded yet\n");
            }
        }
    else
        {
        printf("enter a valid number\n");
        }
    } // while
    list_free(virusList);
}//main