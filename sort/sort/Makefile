sort: test.o mergesort.o
	g++ -o sort test.o mergesort.o

test.o: test.cpp
	g++ -c test.cpp

mergesort.o: mergesort.cpp mergesort.hpp
	g++ -c mergesort.cpp

all: sort ;

clean:
	# -f so this will succeed even if the files don't exist
	rm -f test test.o