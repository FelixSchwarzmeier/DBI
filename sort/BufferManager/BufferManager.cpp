//
//  BufferManager.cpp
//  sort
//
//  Created by Julia Kindelsberger on 25/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//

#include "BufferManager.hpp"
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <tuple>
#include <unistd.h>

// Create a new instance that keeps up to pageCount frames in main memory.
BufferManager::BufferManager(unsigned pageCount)
{
    // max number of page counts
    pC = pageCount;
    mapFrames.reserve(pC);

    //initialize locks
    pthread_rwlock_init(&lock, NULL);
    pthread_mutex_init(&mutex, NULL);
    leastRecentlyUsed = mostRecentlyUsed = NULL;
    
    std::cout << "Buffer Manager created" << std::endl;
}

// Destructor. Write all dirty frames to disk and free all resources.
BufferManager::~BufferManager()
{
    // wait for all to finish
    pthread_rwlock_wrlock(&lock);
    
    // destroy the read-write lock object referenced by rwlock and release any resources used by the lock
    pthread_rwlock_destroy(&lock);
    
    //write all dirty frames to disk // TODO
    for (auto& keyValue: mapFrames)
        keyValue.second.writeChanges();
    
    // close file descriptors
    for (auto& keyValue: mapFileDescriptors)
        close(keyValue.second);
    
    pthread_mutex_destroy(&mutex);
    
    std::cout << "Buffer Manager destroyed" << std::endl;
}

// A method to retrieve frames given a page ID and indicating whether the page will be held exclusively by this thread or not. The method can fail (by throwing an exception) if no free frame is available and no used frame can be freed. The pageID variable is split into a segment ID and the actual page ID. Each page is stored on disk in a file with the same name as its segment ID (e.g., "1").
BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive)
{
    std::cout << "Buffer Manager fixPage " << pageId << " exclusive: " <<exclusive << std::endl;

    BufferFrame* bufferFrame;
    
    // lock the map
    pthread_rwlock_rdlock(&lock);
    
    auto frame = mapFrames.find(pageId);
    
    // check if frame with the pageId is buffered
    if(frame != mapFrames.end()) {
        bufferFrame = &frame->second;
        deleteLeastRecentlyUsed(bufferFrame);
        
    } else {
        
        pthread_rwlock_unlock(&lock);
        pthread_rwlock_wrlock(&lock);
        // TODO check again?
        
        // check if the buffer is full
        if (pC <= mapFrames.size()) {
            BufferFrame* lru = getlLeastRecentlyUsed();
            if (lru == NULL) {
                std::cerr << "Buffer is full" << std::endl;
            }
                
            mapFrames.erase(lru->pageIdentification);

            }
            // page ID (64 bit):
            //   first 16bit: segment (=filename)
            //   48bit: actual page ID
            int fd = getSegmentFd(pageId >> 48);
        
            // create a new frame in the map (with key pageID)
            auto newFrame = mapFrames.emplace(std::piecewise_construct,
                                      std::forward_as_tuple(pageId),
                                      std::forward_as_tuple(fd, pageId)
                                      );
            bufferFrame = &newFrame.first->second;
            
            bufferFrame->currentUsers++;
    }

    // unlock the map
    pthread_rwlock_unlock(&lock);
    
    // lock the frame
    bufferFrame->lockFrame(exclusive);

    return *bufferFrame;
}

// Return a frame to the buffer manager indicating whether it is dirty or not. If dirty, the page manager must write it back to disk. It does not have to write it back immediately, but must not write it back before unfixPage is called.
void BufferManager::unfixPage(BufferFrame& frame, bool isDirty)
{
    std::cout << "Buffer Manager unfix page" << std::endl;
    if(isDirty) {
        frame.setDirty();
    }
    
    frame.writeChanges();
    
    // release the lock on the frame
    frame.unlockFrame();
        
    addLeastRecentlyUsed(&frame);
}


void BufferManager::deleteLeastRecentlyUsed(BufferFrame* frame)
{

    pthread_mutex_lock(&mutex);
    std::cout << "Buffer Manager deleteLeastRecentlyUsed" << std::endl;
    
    // only try to remove this frame from the LRU list if no other user has this
    // frame currently fixed (already removed)
    if(frame->currentUsers++ == 0) {
        if (frame->prev != NULL)
            frame->prev->next = frame->next;
        
        if (frame->next != NULL)
            frame->next->prev = frame->prev;
        
        if (leastRecentlyUsed == frame)
            leastRecentlyUsed = frame->next;
        
        if (mostRecentlyUsed == frame)
            mostRecentlyUsed = frame->prev;
        
        frame->next = frame->prev = NULL;
    }
    pthread_mutex_unlock(&mutex);

}

void BufferManager::addLeastRecentlyUsed(BufferFrame* frame)
{
    pthread_mutex_lock(&mutex);
    std::cout << "Buffer Manager addLeastRecentlyUsed" << std::endl;
    
    // check if frame is used at the moment
    if((--frame->currentUsers) == 0) {
        if(mostRecentlyUsed == NULL) {
            leastRecentlyUsed = frame;
        } else {
            frame->prev  = mostRecentlyUsed;
            mostRecentlyUsed->next = frame;
        }
        mostRecentlyUsed = frame;
    }
    
    pthread_mutex_unlock(&mutex);

}

BufferFrame* BufferManager::getlLeastRecentlyUsed()
{
    pthread_mutex_lock(&mutex);
    std::cout << "Buffer Manager getLeastrecentlyUsed" << std::endl;
    
    BufferFrame* bufferFrame = leastRecentlyUsed;
    if(leastRecentlyUsed != NULL) {
        leastRecentlyUsed = bufferFrame->next;
        bufferFrame->next = NULL;
        
        if(leastRecentlyUsed != NULL)
            leastRecentlyUsed->prev = NULL;
        else
            mostRecentlyUsed = NULL;
    }
    
    pthread_mutex_unlock(&mutex);
    
    return bufferFrame;

}

int BufferManager::getSegmentFd(unsigned segmentId) {
    std::cout << "Buffer Manager getSegmentId " << segmentId << std::endl;
    int fileDescriptor;
    
    // check if the fd exists
    auto entry = mapFileDescriptors.find(segmentId);
    if(entry != mapFileDescriptors.end()) {
        fileDescriptor = entry->second;
    } else {
        
        // create and open the file
        char filename[15];
        sprintf(filename, "%d", segmentId);
        fileDescriptor = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fileDescriptor < 0) {
            std::cerr << "error creating file" << std::endl;
        }
        mapFileDescriptors[segmentId] = fileDescriptor;
    }
    
    return fileDescriptor;
}