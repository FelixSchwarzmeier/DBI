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



#endif