#ifndef CHAININGHT_HPP
#define CHAININGHT_HPP

#include <iostream>
#include <cstdlib>
#include <atomic>
#include <tbb/tbb.h>
#include <unordered_map>

#include <assert.h>

class ChainingHT {
   public:
   // Chained tuple entry
   struct Entry {
      uint64_t key;
      uint64_t value;
      Entry* next;
   };
private:
    uint64_t size;
   uint64_t capacity;

   std::atomic<Entry*>* buckets;

   uint64_t hash(uint64_t key);

public:

   // Constructor
   ChainingHT(uint64_t size);

   // Destructor
   ~ChainingHT();

   // Returns the number of hits
   uint64_t lookup(uint64_t key);

   void insert(Entry* entry);
};


#endif