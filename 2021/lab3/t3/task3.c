#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define lengthSigMax 128
#define bufferSize 10000

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
    char* sig;
} virus;

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

virus* readVirus(FILE* input) { 
    if (input == NULL) {
        printf("signaturs Null\n");
    }
    virus * ans =(virus*)malloc(sizeof(virus));
    fread(ans,sizeof(char),18,input); 
    char * sig = malloc(ans->SigSize);
    fread(sig,1,ans->SigSize,input); 
    ans->sig = sig;
    sig = NULL;
    return ans;
}

virus* readBigVirus(FILE* input) { 
    if (input == NULL) {
        printf("signaturs Null\n");
    }
    virus * ans =(virus*)  malloc(sizeof(virus));
    fread(ans,sizeof(char),18,input);
    printf("%d\n",ans->SigSize);
    ans->SigSize = swap_endian(ans->SigSize);
    printf("%d\n",ans->SigSize);
    char * sig = malloc(ans->SigSize);
    fread(sig,sizeof(char),ans->SigSize,input); 
    ans->sig = sig;
    sig = NULL;
    return ans;
}

void printVirus(virus* virus, FILE* output) {
    char * virusName = "Virus name: ";
    fwrite(virusName,1,strlen(virusName),output);
    fwrite(virus->virusName,1,strlen(virus->virusName),output);
    fprintf(output,"%s","\n");
    char * VirusSize = "Virus Size: ";
    fwrite(VirusSize,1,strlen(VirusSize),output);
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
        link* listPtr =(link*) malloc(sizeof(link));
        listPtr->nextVirus = NULL;
        listPtr->vir = data;
        return listPtr;
    }
    link* newVirus = (link*) malloc(sizeof(sizeof(link)));
    newVirus->vir = data;
    newVirus->nextVirus = NULL;
    link* tmp = virus_list;
    while (tmp->nextVirus != NULL)
    {
        tmp = tmp->nextVirus;
    }
    tmp->nextVirus = newVirus;
    tmp = NULL;
    return virus_list;
}


void list_free(link* virus_list) {
    link* tmp = NULL;
    while(virus_list != NULL){
        tmp = virus_list;
        virus_list = virus_list->nextVirus;
        free(tmp->vir->sig);
        free(tmp->vir);
        tmp ->nextVirus = NULL;
        free(tmp);
    }
    tmp = NULL;
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

void kill_virus(char *filename,int signitureoffset,int signitureSize) {
    printf("file name is : %s\n",filename);
    FILE * infected  = fopen(filename,"r+");
    
    if ( infected == NULL) {
        printf("the File is Null");
    }
    
    unsigned  char * nop = ( unsigned char *) malloc(signitureSize);
    for (int i = 0; i < signitureSize;i++){
        nop[i] = 0x90;
    }   
    fseek(infected,signitureoffset,SEEK_SET);
    fwrite(nop,sizeof(char),signitureSize,infected);
    fseek(infected,0L,SEEK_SET);
    fclose(infected);
    free(nop);
}

unsigned short swap_endian( unsigned short num)
{
    unsigned short out = ((num << 8) | (num >> 8) | (num >> 24) | (num << 24));
    return out;
}


int main(int argc, char **argv) {
    FILE * signature;
    bool isLoaded = false;
    link * virusList = NULL;
    while (1)
    {
        printf("%d %s\n%d %s\n%d %s\n%d %s\n",1,"Load signatures",2,"Print signatures",3,"Detect viruses",4,"Fix file");
        printf("%s\n","Please choose a number:");
        int num = getNum();
        printf("The num is: %d\n",num);
        if(num == 1) {
            printf("%s\n","Please put a signature file name:");
            char sigFile [lengthSigMax];
            fgets(sigFile,lengthSigMax,stdin);
            sigFile[strlen(sigFile) -1] = '\0'; // removing \n
            FILE * signature = fopen(sigFile,"r");  // signatures
            if (signature == NULL) {
                printf("File is null \n");
            }
            int len =sizeFile(signature);
            fseek(signature,4,SEEK_SET);
            char endianType =fgetc(signature);
            if(endianType == 'L') //Little endian
            {
                while(1) {
                if(ftell(signature) == len) {
                    break;
                }
                virus * a = readVirus(signature);
                virusList= list_append(virusList,a);
                }
            }
            else if(endianType == 'B') //Big endian
            {
                while(1) {
                if(ftell(signature) == len) {
                    break;
                }
                virus * a = readBigVirus(signature);
                virusList= list_append(virusList,a);
            }
            }
            
            
            isLoaded = true;
            fclose(signature);
        }
            else if (num == 2) {
                if(isLoaded) {
                    list_print(virusList,stdout); 
                }
                else {
                    printf("The signatures not Loaded\n");
                }
            }
            else if (num == 3) {
                if(isLoaded) {
                    char buffer[bufferSize];
                    FILE* suspectFIle = fopen(argv[1],"r+");
                    unsigned int fileSize = sizeFile(suspectFIle);
                    unsigned int size = minimum(fileSize,bufferSize);
                    fread(buffer,sizeof(char),size,suspectFIle); 
                    detect_virus(buffer,size,virusList,stdout);
                    fclose(suspectFIle);
                }
                else
                {
                    printf("no signatures loaded yet\n");
                }
                }
                else if(num == 4) {
                    if (!isLoaded ) {
                        printf("first load the signatures\n");
                        continue;
                    }
                    printf("enter the starting byte location and the size of the virus signature\n");
                    int loc = getNum();
                    getNum(); // to remove the \n  that stuck in stdin
                    int sigSize  = getNum();
                    kill_virus(argv[1],loc,sigSize);
                }
                else {
                    printf("enter a valid number\n");
                }
    }//while
    list_free(virusList);
} //main