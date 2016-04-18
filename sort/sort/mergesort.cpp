//
//  mergesort.cpp
//  sort
//
//  Created by Julia Kindelsberger on 16/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//

#include "mergesort.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <queue>

#define DEBUG 1

using namespace std;

/* Algorithm example:
 1. Read 100 MB of the data in main memory and sort by some conventional method, like quicksort.
 2. Write the sorted data to disk.
 3. Repeat steps 1 and 2 until all of the data is in sorted 100 MB chunks (there are 900MB / 100MB = 9 chunks), which now need to be merged into one single output file.
 4. Read the first 10 MB (= 100MB / (9 chunks + 1)) of each sorted chunk into input buffers in main memory and allocate the remaining 10 MB for an output buffer. (In practice, it might provide better performance to make the output buffer larger and the input buffers slightly smaller.)
 5. Perform a 9-way merge and store the result in the output buffer. Whenever the output buffer fills, write it to the final sorted file and empty it. Whenever any of the 9 input buffers empties, fill it with the next 10 MB of its associated 100 MB sorted chunk until no more data from the chunk is available. This is the key step that makes external merge sort work externally -- because the merge algorithm only makes one pass sequentially through each of the chunks, each chunk does not have to be loaded completely; rather, sequential parts of the chunk can be loaded as needed.*/

template <typename T>
struct fileBuffer {
    FILE*   sourceFile;
    T*      buffer;
    uint64_t length;
    uint64_t index;
};

struct mergeElement {
    uint64_t value;
    uint64_t srcIndex;
    bool operator < (const mergeElement& a) const {
        return a.value < value;
    }
};

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    
    // check if values are valid
    if(size == 0){
        cerr << "Number of elements is 0!" << endl;
    }
    
    if(memSize < sizeof(uint64_t)) {
        cerr << "Memory size is too small!" << endl;
    }
    
    const uint64_t chunkSize = min(memSize / sizeof(uint64_t), size); // number of values in one chunk
    const uint64_t numberOfChunks = (size + chunkSize-1) / chunkSize; // number of chunks
    vector<FILE*> filesForChunks;
    vector<uint64_t> arrayMemory;
    
    filesForChunks.reserve(numberOfChunks);
    arrayMemory.resize(chunkSize);
    
    //1. Read as much data as will fit into an array memory.
    for(uint64_t i=0; i < numberOfChunks; i++) {
        size_t numberOfValues = chunkSize;
        if(size - i*chunkSize < numberOfValues) {
            numberOfValues = size - i * chunkSize;
            arrayMemory.resize(numberOfValues);
        }
        size_t numberOfBytes = numberOfValues * sizeof(uint64_t);
        cout << "chunk: " << i << "; with numberOfBytes: " << numberOfBytes << endl;
        
        if(read(fdInput, &arrayMemory[0], numberOfBytes) != (ssize_t)numberOfBytes) {
            cerr << "Reading elements from chunk " << i << " failed!" << endl;
            return;
        }
        
        //2. Sort it.
        sort(arrayMemory.begin(), arrayMemory.end());
        
        //3. Write it out to a temporary file (keeping track of name and size and largest record, etc).
        FILE* tempFile = NULL;
        tempFile = tmpfile();
        
        filesForChunks.push_back(tempFile);
        
        fwrite(&arrayMemory[0], sizeof(uint64_t), numberOfValues, tempFile);
    }

    //5. Set up a merge tree for the files written so that you do the minimum of merges.
    priority_queue<mergeElement> prioQueue;
    
    const size_t bufferSize = chunkSize/(numberOfChunks+1);
  
    vector<fileBuffer<uint64_t> > inputBuffers;
    inputBuffers.reserve(numberOfChunks);
    
    //6. Read a line from each of the first (only?) merge phase input files.
    for(uint64_t i=0; i < numberOfChunks; i++) {
        fileBuffer<uint64_t> buffer = {
            filesForChunks[i],
            &arrayMemory[i * bufferSize],
            bufferSize,
            0
        };
        
        rewind(buffer.sourceFile);
        if(fread(buffer.buffer, sizeof(uint64_t), bufferSize, buffer.sourceFile) != bufferSize) {
            cerr << "Reading values from temporary file with index" << i << " failed: ";
            if(feof(buffer.sourceFile))
                cerr << "unexpected EOF!" << endl;
            else
                cerr << "unknown error!" << endl;
            return;
        }
        
        mergeElement mergeElement1;
        mergeElement1.value = buffer.buffer[0];
        mergeElement1.srcIndex = i;
        prioQueue.push(mergeElement1);
        
        buffer.index++;
        buffer.length--;
        
        inputBuffers.push_back(buffer);
    }
    
    // buffer for the output
    uint64_t* outputBuffer = &arrayMemory[numberOfChunks * bufferSize];
    uint64_t outLength = 0;
    
    // empty queue
    while(!prioQueue.empty()) {
        // get min element
        mergeElement minElement = prioQueue.top();
        prioQueue.pop();
        outputBuffer[outLength] = minElement.value;
        outLength++;
        
        // elements to file
        if(outLength == bufferSize-1) {
            if(write(fdOutput, outputBuffer, outLength*sizeof(uint64_t)) != (ssize_t)(outLength*sizeof(uint64_t)))
                cerr << "Writing failed!" << endl;
            outLength = 0;
        }
        
        // fill input buffer
        fileBuffer<uint64_t>& sourceBuffer = inputBuffers[minElement.srcIndex];
        if(sourceBuffer.length > 0) {
            mergeElement mergeElement2;
            mergeElement2.value = sourceBuffer.buffer[sourceBuffer.index];
            mergeElement2.srcIndex = minElement.srcIndex;
            prioQueue.push(mergeElement2);
            sourceBuffer.index++;
            sourceBuffer.length--;
            
            if(sourceBuffer.length == 0) {
                cout << "Filling input buffer number: " << minElement.srcIndex << endl;
                sourceBuffer.index = 0;
                sourceBuffer.length = fread(sourceBuffer.buffer, sizeof(uint64_t), bufferSize, sourceBuffer.sourceFile);
                
                if(sourceBuffer.length < bufferSize) {
                    fclose(sourceBuffer.sourceFile);
                    cout << "Merged all data from chunk number: " << minElement.srcIndex << endl;
                    
                }
            }
        }
    }
    
    if(outLength > 0) {
        if(write(fdOutput, outputBuffer, outLength*sizeof(uint64_t)) != (ssize_t)(outLength*sizeof(uint64_t)))
            cerr << "Writing failed!" << endl;
    }
}
