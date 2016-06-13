#include "Projection.hpp"


Projection::Projection( vector<unsigned int> _col_id, Operator* _op) {
    col_id = _col_id;
    op = _op;
}

void Projection::open() {
    op.open();
}

bool Projection::next() {
    bool has_next = op.next();
    
    if( has_next ) {
        vector<Register> output = op.getOutput();
        for( int i = 0; i < col_id.size(); i++ ) {
            ret.push_back(output[i]);
        }        
    }
}

vector<Register> Projection::getOutput() {
    return ret;
}

void Projection::close() {
    op.close();
}