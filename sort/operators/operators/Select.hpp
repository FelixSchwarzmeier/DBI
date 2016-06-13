#ifndef SELECT_HPP
#define SELECT_HPP


#include "Operator.hpp"
#include <vector>
#include "Register.hpp"

class Select : public Operator {

private:
    Operator* op;
    Register sel;
    unsigned int col_id;
    std::vector<Register> ret;
    
public:
    Select( const Register _sel, unsigned int _col_id, Operator* _op);
    
    void open();
    bool next();
    std::vector<Register> getOutput();
    void close();
};

#endif
