all: FuncA

FuncA: main.o FuncA.o
	g++ -g -Wall main.o FuncA.o -o FuncA.elf 

main.o: main.cpp
	g++ -g -Wall -c main.cpp 

FuncA.o: FuncA.cpp FuncA.h
	g++ -g -Wall -c FuncA.cpp -o FuncA.o

clean: 
	rm -rf -v *.o
	rm -rf -v *.gch


