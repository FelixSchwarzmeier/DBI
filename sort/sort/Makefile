test: test.o mergesort.o
	g++ -o test test.o mergesort.o

test.o: test.cpp
	g++ -c test.cpp

mergesort.o: mergesort.cpp mergesort.hpp
	g++ -c mergesort.cpp

all: test ;

clean:
	# -f so this will succeed even if the files don't exist
	rm -f test test.o