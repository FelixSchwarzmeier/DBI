#ifndef RELATION_hpp
#define RELATION_hpp

#include <stdio.h>
#include <vector>

#include "SPSegment.hpp"

struct metadata {
    bool string;
    unsigned len;
    
    metadata( bool s, unsigned l) {
        string = s;
        len = l;
    }
};


class Relation {
private:
    SPSegment* sps;
    std::vector<metadata> columns;
    
public:
    Relation( SPSegment* _sps );
    void addColumn( bool string, unsigned len);
    
    SPSegment* getSPSegment();
    unsigned getColumns();
    metadata getColumn(int i);
    
};





#endif
