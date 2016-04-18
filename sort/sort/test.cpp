//
//  test.cpp
//  test
//
//  Created by Julia Kindelsberger on 16/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include "mergesort.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>


/*class RandomLong
{
    /// The state
    uint64_t state;
    
public:
    /// Constructor
    explicit RandomLong(uint64_t seed=88172645463325252ull) : state(seed) {}
    
    /// Get the next value
    uint64_t next() { state^=(state<<13); state^=(state>>7); return (state^=(state<<17)); }
};*/

bool sorted (int fdOutput, uint64_t size) {
    
    // set value of fd
    if (lseek(fdOutput, 0, SEEK_SET) < 0) {
        std::cerr << "Rewinding of output file failed!"<< std::endl;
        return false;
    }
    
    size_t count = 0;
    uint64_t previousItem = 0;
    uint64_t currentItem = 0;
    
    count += read(fdOutput, &currentItem, sizeof(uint64_t));
    while (count < size) {
        previousItem = currentItem;
        //std::cout << "sorted values: " << currentItem << std::endl;
        count += read(fdOutput, &currentItem, sizeof(uint64_t));
        if (previousItem > currentItem)
            return false;
    }
    return true;
}

int main(int argc, char* argv[]) { // sort inputFile outputFile memoryBufferInMB
    std::cout << " 0 " << argv[0] << std::endl;
    std::cout << "number of ints 1 " << argv[1] << std::endl;
    std::cout << "outputfile 2 " << argv[2] << std::endl;
    std::cout << "mem 3 " << argv[3] << std::endl;
    
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <file name> <number of elements>" << std::endl;
        return -1;
    }
    /*RandomLong rand;
    unsigned n = atoi(argv[2]);
    std::cout << "n: " <<n << std::endl;
    if (n==0) {
        std::cerr << "invalid length: " << argv[2] << std::endl;
        return -1;
    }
    int fd, ret;
    if ((fd = open(argv[1], O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR)) < 0) {
        std::cerr << "cannot open file '" << argv[1] << "': " << strerror(errno) << std::endl;
        return -1;
    }
    // posix_fallocate not working on OSX
    if ((ret = posix_fallocate(fd, 0, n*sizeof(uint64_t))) != 0)
        std::cerr << "warning: could not allocate file space: " << strerror(ret) << std::endl;
    
    for (unsigned i=0; i<n; ++i) {
        uint64_t x = rand.next();
        //std::cout << "unsorted values " << x << std::endl;
        if (write(fd, &x, sizeof(uint64_t)) < 0) {
            std::cout << "error writing to " << argv[1] << ": " << strerror(errno) << std::endl;
        }
    }*/
    
    int fdInput = open(argv[1], O_RDONLY);
    int fdOutput = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
    uint64_t memSize = atoi(argv[3]) * 1024 * 1024;
    
    
   /* std::ifstream in("random", std::ifstream::ate | std::ifstream::binary);
    uint64_t size =  in.tellg();*/
    
    
    struct stat fs;
    if(stat(argv[1], &fs) < 0) {
        perror("input file stat failed");
        exit(EXIT_FAILURE);
    }
    
    
    
    uint64_t size = (uint64_t)fs.st_size / sizeof(uint64_t);
    
    //size = 625000000;
    std::cout << "size:  " << size << std::endl;
    
    externalSort(fdInput, size, fdOutput, memSize);
    
    if (sorted(fdOutput, size))
        std::cout << "Success!!!! :D" << std::endl;
    else
        std::cout << "ERROR! SORTING FAILED. :(" << std::endl;
    
    close(fdInput);
    close(fdOutput);
    //close(fd);
    return 0;
}




