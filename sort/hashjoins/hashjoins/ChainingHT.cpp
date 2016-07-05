#include "ChainingHT.hpp"

#include "Operator.hpp"

uint64_t ChainingHT::hash(uint64_t key){
    return hashKey(key) % size;
}
   
ChainingHT::ChainingHT(uint64_t _size) {
    size = _size;
    capacity = nextPowOf2(size);
    buckets = new std::atomic<Entry*>[capacity]();
}

ChainingHT::~ChainingHT() {
    delete[] buckets;
}

uint64_t ChainingHT::lookup(uint64_t key) {
    auto& b = buckets[hash(key)];

    uint64_t count = 0;

    Entry* e = b.load();

    while (e){
        if (e->key==key)
        count++;
        e = e->next;
    }     
    return count; 
}

void ChainingHT::insert(Entry* entry) {
    auto& b = buckets[hash(entry->key)];

    entry->next = b.load(std::memory_order_relaxed);
    while(!b.compare_exchange_weak(entry->next, entry,
                                    std::memory_order_release,
                                    std::memory_order_relaxed));
}