
BufferManager: test.o BufferManager.o BufferFrame.o SlottedPage.o SPSegment.o Tid.o Relation.o Record.o
	g++ -o BufferManager test.o BufferManager.o BufferFrame.o SlottedPage.o SPSegment.o Relation.o Record.o

test.o: slottedTest.cpp
	g++ -c -std=c++11 slottedTest.cpp

BufferManager.o: BufferManager.cpp BufferFrame.hpp
	g++ -c -std=c++11 BufferManager.cpp

BufferFrame.o: BufferFrame.cpp
	g++ -c -std=c++11 BufferFrame.cpp
	
SlottedPage.o: SlottedPage.hpp SlottedPage.cpp
	g++ -c -std=c++11 SlottedPage.hpp SlottedPage.cpp
	
SPSegment.o: SPSegment.hpp SPSegment.cpp
	g++ -c -std=c++11 SPSegment.hpp SPSegment

Tid.o: Tid.hpp Tid.cpp
	g++ -c -std=c++11 Tid.hpp Tid.cpp
	
Relation.o: Relation.hpp
	g++ -c -std=c++11 Relation.hpp
	
Record.o: Record.hpp
	g++ -c -std=c++11 Record.hpp
	
all:
	BufferManager;

clean:
	rm -f test test.o



