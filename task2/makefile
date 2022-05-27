# All Targets
all: myshell

myshell: task2.o LineParser.o 
	gcc -g -Wall -m32 -o myshell task2.o LineParser.o 

# Dependancies:
task2.o: task2.c
	gcc -g -Wall -m32 -c -o task2.o task2.c

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c

.PHONY: clean

# Clean the build directory
clean:
	rm -f *.o myshell
