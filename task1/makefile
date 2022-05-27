# All Targets
all: myshell

myshell: task1.o LineParser.o 
	gcc -g -Wall -m32 -o myshell task1.o LineParser.o 

# Dependancies:
task1.o: task1.c
	gcc -g -Wall -m32 -c -o task1.o task1.c

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c

.PHONY: clean

# Clean the build directory
clean:
	rm -f *.o myshell
