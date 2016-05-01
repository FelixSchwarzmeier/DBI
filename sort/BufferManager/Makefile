
BufferManager: test.o BufferManager.o BufferFrame.o
	g++ -o BufferManager test.o BufferManager.o BufferFrame.o

test.o: test.cpp
	g++ -c -std=c++11 test.cpp

BufferManager.o: BufferManager.cpp BufferFrame.hpp
	g++ -c -std=c++11 BufferManager.cpp

BufferFrame.o: BufferFrame.cpp
	g++ -c -std=c++11 BufferFrame.cpp

all:
	BufferManager;

clean:
	rm -f test test.o



