#ifndef OPERATOR_H
#define OPERATOR_H

#include "Register.hpp"

#include <vector>

class Operator {
    
public:
    virtual void open();
    virtual bool next();
    virtual std::vector<Register> getOutput();
    virtual void close();
    
};



#endif