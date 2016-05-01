//
//  BufferManager.hpp
//  sort
//
//  Created by Julia Kindelsberger on 25/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//

#ifndef BufferManager_hpp
#define BufferManager_hpp

#include <stdio.h>
#include <stdint.h>
#include "BufferFrame.hpp"
#include <unordered_map>
#include <pthread.h>

class BufferManager {

    public:
    BufferManager(unsigned pageCount);
    ~BufferManager();
    
    unsigned pC;
    
    BufferFrame& fixPage(uint64_t pageId, bool exclusive);
    void unfixPage(BufferFrame& frame, bool isDirty);
    
    // Hashmap for the frames
    std::unordered_map<uint64_t,BufferFrame> mapFrames;
    
    // Hashmap for the file descriptors
    std::unordered_map<unsigned, int> mapFileDescriptors;
    
    pthread_rwlock_t lock;
    pthread_mutex_t mutex;
    
    BufferFrame* leastRecentlyUsed;
    BufferFrame* mostRecentlyUsed;
    
    int getSegmentFd(unsigned segmentId);
    
    void addLeastRecentlyUsed(BufferFrame* frame);
    void deleteLeastRecentlyUsed(BufferFrame* frame);
    BufferFrame* getlLeastRecentlyUsed();
};

#endif /* BufferManager_hpp */
