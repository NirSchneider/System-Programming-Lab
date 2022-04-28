all: exec

libs: c-lib

c-lib: LineParser.c
	# flags meanings:
	# -g = debug symbols
	# -m32 = compile to x86_32 architecture (32bit)
	# -c = Don't link (i.e., create an object file, not an executable)
	# -o numbers.o = output compiled file to numbers.o
	gcc -g -m32 -c -o LineParser.o LineParser.c

exec: task0.c libs
	gcc -g -m32 -c -o task0.o task0.c
	gcc -g -m32  task0.o LineParser.o -o task0
	rm task0.o LineParser.o


.PHONY: clean
clean:
	rm -rf ./*.o task0
