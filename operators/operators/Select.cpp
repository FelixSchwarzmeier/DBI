#include "Select.hpp"


Select::Select( const Register _sel, unsigned int _col_id, Operator* _op) {
    col_id = _col_id;
    sel = _sel;
    op = _op;
}

void Select::open() {
    op.open();
}

bool Select::next() {
    bool has_next = op.next();
    
    if( has_next  ) {
        ret = op.getOutput();
        
        while( ret[col_id] != sel && has_next ) {
            has_next = op.next();
            ret = op.getOutput();
        }
    }
}

vector<Register> Select::getOutput() {
    return ret;
}

void Select::close() {
    op.close();
}

