//
//  test.cpp
//  sort
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

bool isSorted (int fdOutput, uint64_t size){
    
    size_t counter = 0;
    uint64_t previousNumber = 0;
    uint64_t currentNumber = 0;
    
    if(lseek(fdOutput, 0, SEEK_SET) < 0) {
        std::cerr << "Rewinding failed!" << std::endl;
    }
    
    counter += read(fdOutput, &currentNumber, sizeof(uint64_t));
    
    while (counter < size) {
        previousNumber = currentNumber;
        //std::cout << "sorted values: " << currentNumber << std::endl;
        counter += read(fdOutput, &currentNumber, sizeof(uint64_t));
        if(previousNumber > currentNumber) {
            return false;
        }
    }
    return true;
}


int main(int argc, char* argv[]) {
    
    if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " <inputFile> <OutputFile> <memoryBufferinMB>" << std::endl;
        return -1;
    }
    
    int fdInput = open(argv[1], O_RDONLY);
    int fdOutput = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
    uint64_t memSize =  1024 * 1024 * atoi(argv[3]); // to B

    struct stat fs;
    if(stat(argv[1], &fs) < 0) {
        perror("Input file stat failed!");
        exit(EXIT_FAILURE);
    }
    
    uint64_t size = (uint64_t)fs.st_size / sizeof(uint64_t); // get size of file
    
    // sort inputFile outputFile memoryBufferInMB
    externalSort(fdInput, size, fdOutput, memSize);
    
    if (isSorted(fdOutput, size))
        std::cout << "Sorting has been successfull! :D" << std::endl;
    else
        std::cout << "ERROR! Sorting failed. :(" << std::endl;
    
    close(fdInput);
    close(fdOutput);
    return 0;
}




