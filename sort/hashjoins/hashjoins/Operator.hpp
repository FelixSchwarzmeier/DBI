#ifndef OPERATOR_H
#define OPERATOR_H

#include "Register.hpp"

#include <vector>

class Operator {
    
public:
    virtual void open() = 0;
    virtual bool next() = 0;
    virtual std::vector<Register> getOutput() = 0;
    virtual void close() = 0;
    
};

inline uint64_t nextPowOf2(unsigned long v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

inline uint64_t hashKey(uint64_t k) {
   // MurmurHash64A
   const uint64_t m = 0xc6a4a7935bd1e995;
   const int r = 47;
   uint64_t h = 0x8445d61a4e774912 ^ (8*m);
   k *= m;
   k ^= k >> r;
   k *= m;
   h ^= k;
   h *= m;
   h ^= h >> r;
   h *= m;
   h ^= h >> r;
   return h|(1ull<<63);
}



#endif