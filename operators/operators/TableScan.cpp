#include "TableScan.hpp"


TableScan::TableScan(SPSegment* _sps) {
    sps = _sps;
    t = TID(0,0);
}

void TableScan::open() {
    //open SPSegement
    //nichts zu tun
}

bool TableScan::next() {
    current = t;
    if( t.pageid < sps->get_pageIdCounter() ) {
        if( t.slotid < sps->get_slotIdCounter(t.pageid)) {
            t.slotid++;
        } else {
            t.pageid++;
            t.slotid = 0;
        }
        return true;
    } else {
        return false;
    }
    
}

vector<Register> TableScan::getOutput() {
    return sps->lookup(current);
}

void TableScan::close() {
    //close notwendig?
}
