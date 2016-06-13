#include "TableScan.hpp"


TableScan::TableScan(Relation* _rel) {
    rel = _rel;
    sps = rel->getSPSegment();
    t = TID(0,0);
}

void TableScan::open() {
    //open SPSegement
    //nichts zu tun
}

bool TableScan::next() {
    current = t;
    if( t.pageId < sps->get_pageIdCounter() ) {
        if( t.slotId < sps->get_slotIdCounter(t.pageId)) {
            t.slotId++;
        } else {
            t.pageId++;
            t.slotId = 0;
        }
        return true;
    } else {
        return false;
    }
    
}

std::vector<Register> TableScan::getOutput() {
    std::vector<Register> ret;
    unsigned o = 0;
    Record rec = sps->lookup(current);
    for( unsigned i = 0; i < rel->getColumns(); i++) {
        metadata md = rel->getColumn(i);
        if( md.string ) {
            std::string s(rec.getData()+o, rec.getLen()); 
            ret.push_back(Register(s));
            o += rec.getLen();
        } else {
            int i = *((int*)rec.getData());
            ret.push_back(Register(i));
            o += rec.getLen();
        }
    }
    return ret;
}

void TableScan::close() {
    //close notwendig?
}
