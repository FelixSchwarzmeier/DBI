#include "Select.hpp"


Select::Select( Register _sel, unsigned int _col_id, Operator* _op) : sel(_sel){
    col_id = _col_id;
    op = _op;
}

void Select::open() {
    op->open();
}

bool Select::next() {
    bool has_next = op->next();
    
    if( has_next  ) {
        ret = op->getOutput();
        
        while( !(ret[col_id] == sel) && has_next ) {
            has_next = op->next();
            ret = op->getOutput();
        }
    }
    return has_next;
}

std::vector<Register> Select::getOutput() {
    return ret;
}

void Select::close() {
    op->close();
}

