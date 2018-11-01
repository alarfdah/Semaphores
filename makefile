all:
	gcc -g -Wall a3_processes.c -lpthread -o a3_processes
	gcc -g -Wall a3_threads.c -lpthread -o a3_threads


run_processes:
	./a3_processes

run_threads:
	./a3_threads

