#ifndef CHAININGLOCKINGHT_HPP
#define CHAININGLOCKINGHT_HPP

#include <iostream>
#include <cstdlib>
#include <atomic>
#include <tbb/tbb.h>
#include <unordered_map>

#include <assert.h>

using namespace tbb;

class ChainingLockingHT {
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

   struct Bucket {
      Entry* head;
      tbb::mutex bMutex;
   };

   Bucket* buckets;

   uint64_t hash(uint64_t key);

public:

   // Constructor
   ChainingLockingHT(uint64_t _size);
   
   // Destructor
   ~ChainingLockingHT();

   // Returns the number of hits
   uint64_t lookup(uint64_t key);

   void insert(Entry* entry);
};

#endif