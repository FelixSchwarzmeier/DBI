#include "ChainingLockingHT.hpp"

#include "Operator.hpp"

using namespace tbb;

uint64_t ChainingLockingHT::hash(uint64_t key){
    return hashKey(key) % size;
}
   
ChainingLockingHT::ChainingLockingHT(uint64_t _size) {
    size = _size;
    buckets = new Bucket[nextPowOf2(size)]();
}

ChainingLockingHT::~ChainingLockingHT() {
    delete[] buckets;
}

// Returns the number of hits
uint64_t ChainingLockingHT::lookup(uint64_t key) {
    auto& b = buckets[hash(key)];

    uint64_t count =0;

    b.bMutex.lock();
    Entry* e = b.head;
    b.bMutex.unlock();
    while (e){
        if (e->key==key)
        count++;
        e = e->next;
    }     
    return count; 
}

void ChainingLockingHT::insert(Entry* entry) {
    auto& b = buckets[hash(entry->key)];

    b.bMutex.lock();
    entry->next = b.head;
    b.head = entry;
    b.bMutex.unlock();
}