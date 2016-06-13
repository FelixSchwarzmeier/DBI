#ifndef PROJECTION_HPP
#define PROJECTION_HPP


#include "Operator.hpp"
#include <vector>

class Projection : public Operator {
private: 
    Operator* op;
    std::vector<unsigned int> col_id;
    std::vector<Register> ret;
public:
    Projection( std::vector<unsigned int> _col_id, Operator* _op);
    
    void open();
    bool next();
    std::vector<Register> getOutput();
    void close();
    
};

#endif