
CXXFLAGS+= -ltbb -std=c++11 -Wall

SOURCES = $(wildcard *.cpp)
#SOURCES = test.cpp Btree.cpp BufferManager.cpp BufferFrame.cpp Tid.cpp
HEADERS = $(wildcard *.hpp)
OBJECTS = $(SOURCES:.cpp=.o)
LDFLAGS = -pthread -lpthread -ltbb


Hashjoins: $(OBJECTS)
	g++ -Wall $(LDFLAGS) $(OBJECTS) -o Hashjoins

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

all:
	Hashjoins;

clean:
	rm -f test *.o

