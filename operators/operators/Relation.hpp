#ifndef RELATION_hpp
#define RELATION_hpp

#include <stdio.h>

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
    vector<metadata> columns;
    unsigned total_len;
    
public:
    Relation( SPSegments _sps );
    addColumn( bool string, unsigned len);
    
}





#endif
