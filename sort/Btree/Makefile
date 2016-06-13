
CXXFLAGS+= -std=c++11 -Wall

#SOURCES = $(wildcard *.cpp)
SOURCES = test.cpp Btree.cpp BufferManager.cpp BufferFrame.cpp Tid.cpp
HEADERS = $(wildcard *.hpp)
OBJECTS = $(SOURCES:.cpp=.o)
LDFLAGS = -pthread -lpthread


Btree: $(OBJECTS)
	g++ -Wall $(LDFLAGS) $(OBJECTS) -o Btree

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

all:
	Btree;

clean:
	rm -f test *.o

