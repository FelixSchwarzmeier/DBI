#ifndef PRINT_HPP
#define PRINT_HPP


#include "Operator.hpp"
#include <vector>
#include <iostream>

class Print : public Operator {
private:
    Operator* op;
public:
    Print( Operator* _op );
    
    void open();
    bool next();
    std::vector<Register> getOutput();
    void close();
    
};

#endif
