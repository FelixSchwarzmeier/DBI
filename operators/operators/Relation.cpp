#include "Relation.hpp"

Relation::Relation( SPSegment* _sps) {
    sps = _sps;
}

void Relation::addColumn( bool string, unsigned len) {
    columns.push_back(metadata(string, len));
}

SPSegment* Relation::getSPSegment() {
    return sps;
}

unsigned Relation::getColumns() {
    return columns.size();
}

metadata Relation::getColumn(int i) {
    return columns[i];
}