all: task3

LineParser.o: LineParser.c
	gcc -m32 -Wall -g -c -o LineParser.o LineParser.c

task3:LineParser.o task3.o
	gcc -m32 -Wall -g -o task3 task3.o LineParser.o
	
task3.o: task3.c
	gcc -m32 -Wall -g -c -o task3.o task3.c
 
.PHONY:

clean:
	rm -f *.o task3

