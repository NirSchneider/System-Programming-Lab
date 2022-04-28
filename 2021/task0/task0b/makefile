all: exec

exec: printNum.c 
	gcc -g -m32 -c -o printNum.o printNum.c
	gcc -g -m32  printNum.o -o printNum
	rm printNum.o 

.PHONY: clean
clean:
	rm -rf ./*.o printNum
