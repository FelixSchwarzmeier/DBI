#ifndef TABLESCAN_HPP
#define TABLESCAN_HPP


#include "Operator.hpp"
#include "SPSegment.hpp"

class TableScan : class Operator {
private:
    SPSegment sps;
    TID t;
    TID current;
pulic:
    TableScan( SPSegment* _sps );
    
};

#endif
