all: task1

task1d: task1.o
	gcc -m32 -g -Wall -o task1 task1.o
	rm *.o

task1d.o: task1d.c
	gcc -m32 -g -Wall -c -o task1.o task1.c
	
.PHONY: clean
clean:
	rm -f task1