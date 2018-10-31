all:
	gcc -g -Wall *.c -lpthread -o a1

run: 
	./a1

debug:
	gdb ./a1