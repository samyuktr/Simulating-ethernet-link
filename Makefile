all: main.o 
	gcc -g -Wall main.o -o pa1 -lm -lrt
main.o: main.c mm1.h
	gcc -c -g -Wall main.c 
clean:
	rm *.o
	rm pa1
