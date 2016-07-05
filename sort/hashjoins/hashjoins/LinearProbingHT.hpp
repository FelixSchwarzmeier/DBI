#ifndef LINEARPROBINGHT_HPP
#define LINEARPROBINGHT_HPP

#include <iostream>
#include <cstdlib>
#include <atomic>
#include <tbb/tbb.h>
#include <unordered_map>

#include <assert.h>


class LinearProbingHT {
public:
    // Entry
    struct Entry {
        uint64_t key;
        uint64_t value;
        std::atomic<bool> marker;
    };
    
private:
    uint64_t size;
    uint64_t capacity;

    Entry* lp_a;

public:
    
    // Constructor
    LinearProbingHT(uint64_t size);

    // Destructor
    ~LinearProbingHT();

    // Returns the number of hits
    uint64_t lookup(uint64_t key);

    void insert(uint64_t key, uint64_t value);
};


#endif