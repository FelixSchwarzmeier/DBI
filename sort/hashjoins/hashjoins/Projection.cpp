#include "Projection.hpp"


Projection::Projection( std::vector<unsigned int> _col_id, Operator* _op) {
    col_id = _col_id;
    op = _op;
}

void Projection::open() {
    op->open();
}

bool Projection::next() {
    bool has_next = op->next();
    
    if( has_next ) {
        std::vector<Register> output = op->getOutput();
        ret = std::vector<Register>();
        for( unsigned i = 0; i < col_id.size(); i++ ) {
            ret.push_back(output[i]);
        }        
    }
    return has_next;
}

std::vector<Register> Projection::getOutput() {
    return ret;
}

void Projection::close() {
    op->close();
}