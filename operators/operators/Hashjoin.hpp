#ifndef HASHJOIN_HPP
#define HASHJOIN_HPP


#include "Operator.hpp"

class HashJoin : class Operator {
private:
    Operator* op1;
    Operator* op1;
    unsigned col1;
    unsigned col2;
    
    
    std::vector<Register> ret;
    std::queue<vector<Register>> buffer;
    
    std::unordered_multimap<Register, vector<Register>> small_table;
    
pulic:
    HashJoin( Operator* _op1; unsigned _col1, Operator* _op2, unsigned _col2 );
    
};

#endif
