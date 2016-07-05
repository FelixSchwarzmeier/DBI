#include "Print.hpp"

Print::Print(Operator* _op) {
    op = _op;
}


void Print::open() {
    op->open();
}

bool Print::next() {
    //ausgeben
    
    bool has_next = op->next();
    
    if( has_next ) {
        std::vector<Register> output = op->getOutput();
        
        for( unsigned i = 0; i < output.size(); i++ ) {
            if(output[i].is_num()) {
                std::cout << output[i].getInteger() << std::endl;
            } else {
                std::cout << output[i].getString() << std::endl;
            }
        }        
    }
    
    return has_next;
}

std::vector<Register> Print::getOutput() {
    return op->getOutput();
}

void Print::close() {
    op->close();
}