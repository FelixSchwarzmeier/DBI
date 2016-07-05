#include "LinearProbingHT.hpp"

#include "Operator.hpp"

LinearProbingHT::LinearProbingHT(uint64_t _size) {
    size = _size;
    capacity = nextPowOf2(size);
    lp_a = new Entry[capacity]();
}

LinearProbingHT::~LinearProbingHT() {
    delete[] lp_a;    
}

// Returns the number of hits
uint64_t LinearProbingHT::lookup(uint64_t key) {
    size_t start = hashKey(key) % size;
    size_t i = start;

    uint64_t count =0;

    while (true){
        auto& e = lp_a[i];

        if (!e.marker ){
            return count;
        } else {
            //Falls Schlüssel gleich inc um 1
            count += e.key == key ? 1 : 0;
        }

        i = (i+1) % size;
        
        if (i==start)
        return count;
    }
}

void LinearProbingHT::insert(uint64_t key, uint64_t value) {
    size_t start = hashKey(key) % size;
    size_t i = start;
    while (true){
        auto& e = lp_a[i];
        bool cmp = false;
        if (e.marker.compare_exchange_weak(cmp, true)){
            e.key = key;
            e.value = value;
            return;
        }

        i = (i+1) % size;
        
        if (i==start)
        throw std::runtime_error("Kapazitaet ausgeschnutzt");
    }
}