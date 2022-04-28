    #include <stdio.h>
    #include <stdbool.h>

    char functionEnc(char c,char ** argv, int enc, bool positiveEnc);
    char functionLowcase(char c);

    int main(int argc,char **argv) {
        FILE *streamError = stderr;
        FILE *streamout;

        char bugModeChar[] = "-D";  
        bool bug = isBugModeOn(argc,argv,bugModeChar);
        if(bug){
            printMode(argc,argv,streamError);
        }

        int enc =findEnc(argc,argv);
        bool positiveEnc = true;
        if(enc!=0) {
            if(argv[enc][0] == '-') {
                positiveEnc = false;
            }
        }

        int oFile = findFile(argc,argv);
        if (oFile!=0) {
            streamout = fopen(argv[oFile] + 2, "w");
        }
        else
        {
            streamout = stdout;
        }
        
        int counter = 0;
        while(true)
        {
            char c = fgetc(stdin);
            if (feof(stdin)) {
                break;
            }
            if(c>'A' -1 && c<'Z' + 1){
                counter ++;
            }
            if (bug && c != 'n') {
                fprintf(streamError,"%d ", c);
            }
            if (enc!=0) {
                c = functionEnc(c,argv, enc,positiveEnc);
            }
            else {
                c = functionLowcase(c);
            }
            
            if (bug && c != 'n') {
                fprintf(streamError,"%d %c",c,'\n');
            }
             if(bug && c == 'n'){
                fprintf(streamError,"\nthe number of letters: %d\n\n", counter);
            }
            fputc(c,streamout);
        
        }
    fclose(streamout);
    return 0;
    }

    int isBugModeOn(int argc,char **argv, char * bugModeChar) {
        for (int i = 1; i<argc; i++) {
            if(strncmp(argv[i],bugModeChar) == 0) {
                return true;
            }
        }
        return false;
    }

    void printMode(int argc,char ** argv,FILE *stream) {
        for(int i =1; i<argc; i++) {
            fprintf(stream,"%s\n",argv[i]);
        }
    }

    int findFile(int argc,char **argv) {
        if(argc <2) {
            return 0;
        }
        for (int i=1; i<argc;i++) { 
            if (argv[i][0] == '-' && argv[i][1]=='o') {
                return i;
            }
        }
        return 0;
    }

    int findEnc (int argc, char **argv) {
        if (argc<2) {
            return 0;
        }
        for (int i =1; i<argc; i++) {
            if ((argv[i][0]== '+'|| argv[i][0]== '-')&& argv[i][1] =='e') {
                return i;
            }
        }
        return 0;
    }

    char functionEnc(char c,char ** argv, int enc, bool encPlus) {
        int value = argv[1][2] - '0';
            if (encPlus == false)
            {
                value = value*(-1);
            }
            c = c + value;
            return c;
    }
    char functionLowcase(char c) {
        int maxValueUpChar = 'Z' + 1;
        int minValueUpChar = 'A' -1;
        int diffrentLowUp = 'z'-'Z';
        if(c>minValueUpChar && c<maxValueUpChar)
        {
            c = c + diffrentLowUp;
        }
        return c;
    }