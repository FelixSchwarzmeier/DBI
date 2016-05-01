//
//  BufferFrame.hpp
//  sort
//
//  Created by Julia Kindelsberger on 25/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//

#ifndef BufferFrame_hpp
#define BufferFrame_hpp

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <iostream>

enum state_t {
    New,   // no data loaded
    Clean, // data loaded and no data changed
    Dirty  // data loaded and changed
};

class BufferFrame {
    
    public:
    
    BufferFrame(uint64_t pageId, int fileDescriptor);
     ~BufferFrame();
    void* getData();
    uint64_t pageIdentification;
    // offset in the segment file
    off_t offset;
    void* data;
    int fileDescriptor;
    void setDirty() {
        std::cout << "Buffer Frame setDirty" << std::endl;
        state = state_t::Dirty; }
    
    // the state of the page
    state_t state;
    void writeChanges();
    
    BufferFrame* prev;
    BufferFrame* next;
    unsigned currentUsers;
    void lockFrame(bool exclusive);
    pthread_rwlock_t rwlock;
    
    void unlockFrame() {
        std::cout << "Buffer Frame unlock" << std::endl;
        pthread_rwlock_unlock(&rwlock);
    }

};

#endif /* BufferFrame_hpp */