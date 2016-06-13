#ifndef PROJECTION_HPP
#define PROJECTION_HPP


#include "Operator.hpp"


class Projection : class Operator {
private: 
    Operator* op;
    vector<unsigned int> col_id;
    vector<Register> ret;
public:
    Projection( vector<unsigned int> _col_id, Operator* _op);
    
};

#endif