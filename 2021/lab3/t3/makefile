all : exec

exec: task3.c
	gcc -g -m32 -c -o task3.o task3.c
	gcc -g -m32 task3.o -o task3
	rm task3.o

clean:
	rm -rf ./*.o task3