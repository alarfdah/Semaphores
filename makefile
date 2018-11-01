all:
	gcc -g -Wall a3_processes.c -lpthread -o a3_processes

run: 
	./a3_processes

debug:
	gdb ./a3_processes