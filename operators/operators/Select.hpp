#ifndef SELECT_HPP
#define SELECT_HPP


#include "Operator.hpp"


class Select : class Operator {
private:
    Register sel;
    unsigned int col_id;
    vector<Register> ret;
    
public:
    Select( const Register sel, * _op);
};

#endif
