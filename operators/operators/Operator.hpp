#ifndef OPERATOR_H
#define OPERATOR_H

#include "Register.hpp"

class Operator {
    
public:
    virtual void open();
    virtual bool next();
    virtual vector<Register> getOutput();
    virtual void close();
    
};






class HashJoin: class Operator {
    
};


#endif