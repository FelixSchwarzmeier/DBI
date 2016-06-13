#include "relation.hpp"

Relation::Relation() {
    sps = _sps;
}

void Relation::addColumn( bool string, unsigned len) {
    columns.push_back(metadata(string, len));
}