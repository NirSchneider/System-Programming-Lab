all: exec

libs: asm-lib c-lib

asm-lib: start.s
	# flags meanings:
	# -g = debug symbolx
	# -f elf = output format (ELF 32 bit)
	# -o add.o = write assembled file to add.o
	nasm -g -f elf -o start.o start.s

	# flags meanings:
	# -g = debug symbols
	# -m32 = compile to x86_32 architecture (32bit)
	# -c = Don't link (i.e., create an object file, not an executable)
	# -o numbers.o = output compiled file to numbers.o
	
c-lib: util.c task2b.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector task2b.c -o task2b.o

exec: libs
	ld -m elf_i386 start.o task2b.o util.o -o task2b
	rm start.o task2b.o util.o

.PHONY: clean
clean:
	rm -rf ./*.o task2b
