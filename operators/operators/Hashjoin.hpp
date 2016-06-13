#ifndef HASHJOIN_HPP
#define HASHJOIN_HPP


#include "Operator.hpp"
#include <vector>
#include <queue>
#include <unordered_map>

class HashJoin : public Operator {
private:
    Operator* op1;
    Operator* op2;
    unsigned col1;
    unsigned col2;
    
    
    std::vector<Register> ret;
    std::queue<std::vector<Register>> buffer;
    
    std::unordered_multimap<Register, std::vector<Register>> small_table;
    
pulic:
    HashJoin( Operator* _op1; unsigned _col1, Operator* _op2, unsigned _col2 );
    
    void open();
    bool next();
    std::vector<Register> getOutput();
    void close();
    
};

#endif
