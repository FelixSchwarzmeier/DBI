//
//  mergesort.cpp
//  test
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

struct mergeItem {
    uint64_t value;
    uint64_t srcIndex;
    
    bool operator < (const mergeItem& a) const {
        return a.value < value;
    }
};

template <typename T>
struct fileBuffer {
    FILE*    srcFile;
    T*       buffer;
    uint64_t length;
    uint64_t index;
};

/* 1. Read as much data as will fit into an array memory.
 2. Sort it.
 3. Write it out to a temporary file (keeping track of name and size and largest record, etc).
 4. Go back to step 1 until you reach the end of the data.
 5. Set up a merge tree for the files written so that you do the minimum of merges.
 6. Read a line from each of the first (only?) merge phase input files.
 7. Write the smallest (for an ascending sort) to the next temporary file (or the final file).
 8. Get a new record to replace the one just written.
 9. Go back to step 7 until there is no more data to read.
 10. Go back to step 6 to continue the merge until you're done.*/

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    if(size == 0) {
        cerr << "Size is 0!" << endl;
        return;
    }
    
    if(memSize < sizeof(uint64_t)) {
        cerr << "You need more memory for sorting!" << endl;
        return;
    }
    
    
    
    // how many values fit into one chunk
    const uint64_t chunkSize = min(memSize / sizeof(uint64_t), size);
    
    // number of chunks we must split the input into
    const uint64_t numChunks = (size + chunkSize-1) / chunkSize;
    
    //if(chunkSize == 0)
    
#ifdef DEBUG
    cout << "filesize: " << (size*sizeof(uint64_t)) <<
    " B, memSize: " << memSize <<
    " B, chunkSize: " << chunkSize << ", numChunks: " << numChunks << endl;
#endif
    
    vector<FILE*> chunkFiles;
    chunkFiles.reserve(numChunks);
    
    vector<uint64_t> memBuf;
    memBuf.resize(chunkSize);
    
    //1. Read as much data as will fit into an array memory.
    for(uint64_t i=0; i < numChunks; i++) {
        size_t valuesToRead = chunkSize;
        if(size - i*chunkSize < valuesToRead) {
            valuesToRead = size - i*chunkSize;
            memBuf.resize(valuesToRead);
        }
        size_t bytesToRead = valuesToRead * sizeof(uint64_t);
        
#ifdef DEBUG
        cout << "chunk #" << i << " bytesToRead: " << bytesToRead << endl;
#endif
        
        if(read(fdInput, &memBuf[0], bytesToRead) != (ssize_t)bytesToRead) {
            perror("Reading chunk of input failed");
            return;
        }
        
        //2. Sort it.
        sort(memBuf.begin(), memBuf.end());
        
        //3. Write it out to a temporary file (keeping track of name and size and largest record, etc).
        FILE* tmpf = NULL;
        tmpf = tmpfile();
        if(tmpf == NULL) {
            cerr << "Creating a temporary file failed!" << endl;
            return;
        }
        chunkFiles.push_back(tmpf);
        
        fwrite(&memBuf[0], sizeof(uint64_t), valuesToRead, tmpf);
        if(ferror(tmpf)) {
            cerr << "Writing chunk to file failed!" << endl;
        }
    }
    
    //5. Set up a merge tree for the files written so that you do the minimum of merges.
    
    // priority queue used for n-way merging values from n chunks
    priority_queue<mergeItem> queue;
    
    // We reuse the already allocated memory from the memBuf and split the
    // available memory between numChunks chunk buffers and 1 output buffer.
    const size_t bufSize = chunkSize/(numChunks+1);
    
    // input buffers (from chunks)
    vector<fileBuffer<uint64_t> > inBufs;
    inBufs.reserve(numChunks);
    
    //6. Read a line from each of the first (only?) merge phase input files.
    for(uint64_t i=0; i < numChunks; i++) {
        fileBuffer<uint64_t> buf = {
            chunkFiles[i],      /* srcFile */
            &memBuf[i*bufSize], /* buffer  */
            bufSize,            /* length  */
            0                   /* index   */
        };
        
        rewind(buf.srcFile);
        if(fread(buf.buffer, sizeof(uint64_t), bufSize, buf.srcFile) != bufSize) {
            cerr << "Reading values from tmp chunk file #" << i << " failed: ";
            if(feof(buf.srcFile))
                cerr << "unexpected EOF!" << endl;
            else
                cerr << "unknown error!" << endl;
            return;
        }
        
        mergeItem mergeItem1;
        
        mergeItem1.value = buf.buffer[0];
        mergeItem1.srcIndex = i;
        queue.push(mergeItem1);
        
        //queue.push(mergeItem{buf.buffer[0], i});
        buf.index++;
        buf.length--;
        
        inBufs.push_back(buf);
    }
    
    // output buffer
    uint64_t* outBuf = &memBuf[numChunks*bufSize];
    uint64_t outLength = 0;
    
    while(!queue.empty()) {
        // put min item in buffer
        mergeItem top = queue.top();
        queue.pop();
        outBuf[outLength] = top.value;
        outLength++;
        
        // flush buffer to file, if buffer is full
        if(outLength == bufSize-1) {
            if(write(fdOutput, outBuf, outLength*sizeof(uint64_t)) != (ssize_t)(outLength*sizeof(uint64_t)))
                cerr << "Writing to out file failed!" << endl;
            outLength = 0;
        }
        
        // refill input buffer, from which the value was taken
        // if length == 0 then the chunk was completely read
        fileBuffer<uint64_t>& srcBuf = inBufs[top.srcIndex];
        if(srcBuf.length > 0) {
            mergeItem mergeItem2;
            mergeItem2.value = srcBuf.buffer[srcBuf.index];
            mergeItem2.srcIndex = top.srcIndex;
            queue.push(mergeItem2);
            //queue.push(mergeItem{srcBuf.buffer[srcBuf.index], top.srcIndex});
            srcBuf.index++;
            srcBuf.length--;
            if(srcBuf.length == 0) {
#ifdef DEBUG
                cout << "refilling inBuf #" << top.srcIndex << endl;
#endif
                
                srcBuf.index = 0;
                srcBuf.length = fread(srcBuf.buffer, sizeof(uint64_t), bufSize, srcBuf.srcFile);
                
                // Close the file stream when it is completely read
                if(srcBuf.length < bufSize) {
                    fclose(srcBuf.srcFile);
#ifdef DEBUG
                    cout << "merged all data from chunk #" << top.srcIndex << endl;
#endif
                }
            }
        }
    }
    
    // flush rest, if output buffer is not already empty
    if(outLength > 0) {
        if(write(fdOutput, outBuf, outLength*sizeof(uint64_t)) != (ssize_t)(outLength*sizeof(uint64_t)))
            cerr << "Writing to out file failed!" << endl;
    }
}

