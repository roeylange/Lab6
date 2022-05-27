# All Targets
all: myshell

myshell: task3.o LineParser.o 
	gcc -g -Wall -m32 -o myshell task3.o LineParser.o 

# Dependancies:
task3.o: task3.c
	gcc -g -Wall -m32 -c -o task3.o task3.c

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c

.PHONY: clean

# Clean the build directory
clean:
	rm -f *.o myshell
