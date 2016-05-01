//
//  BufferFrame.cpp
//  sort
//
//  Created by Julia Kindelsberger on 25/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//

#include "BufferFrame.hpp"
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>


const size_t blocksize = 8 * 1024;

BufferFrame::BufferFrame(uint64_t pageId, int fd){
    pthread_rwlock_init(&rwlock, NULL);
    
    pageIdentification = pageId;
    data = NULL;
    state  = state_t::New;
    offset = blocksize * (pageId & 0x0000FFFFFFFFFFFF);
    fileDescriptor = fd;
    
    prev = next = NULL;
    currentUsers = 0;
    
    std::cout << "Buffer Frame created" << std::endl;

}

BufferFrame::~BufferFrame() {
    lockFrame(true); // get exclusive lock / wait until all tasks are finished
    pthread_rwlock_destroy(&rwlock);
    
    // write modified data back to disk
    writeChanges();
    
    // deallocate data, if necessary
    if (data != NULL) {
        free(data);
        data = NULL;
    }
    
    std::cout << "Buffer Frame destroyed" << std::endl;
}

// A buffer frame should offer a method giving access to the buffered page. Except for the buffered page, BufferFrame objects can also store control information (page ID, dirtyness, ...).
void* BufferFrame::getData() {
    
    // in-memory buffer
    data = malloc(blocksize);

    if (data != NULL) {
        
    // read data from file to buffer
    pread(fileDescriptor, data, blocksize, offset);
        
    state = state_t::Clean;
    
    std::cout << "Buffer Frame getData " << data << std::endl;
    
         }
    else {
        std::cerr << "data is null" << std::endl;
    }
    return data;
}

void BufferFrame::writeChanges() {
    std::cout << "Buffer Frame writeChanges" << std::endl;
    // write data to disk if it is dirty
    if (state == state_t::Dirty) {
        pwrite(fileDescriptor, data, blocksize, offset);
        state = state_t::Clean;
    }
}

void BufferFrame::lockFrame(bool exclusive) {
    std::cout << "Buffer Frame lock. exclusive: " << exclusive  << std::endl;
    if (exclusive)
        pthread_rwlock_wrlock(&rwlock);
    else
        pthread_rwlock_rdlock(&rwlock);
}
