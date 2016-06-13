#ifndef PRINT_HPP
#define PRINT_HPP


#include "Operator.hpp"


class Print : class Operator {
private:
    operator* op;
public:
    Print( Operator* _op );
    
};

#endif
