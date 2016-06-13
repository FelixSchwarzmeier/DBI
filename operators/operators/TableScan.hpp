#ifndef TABLESCAN_HPP
#define TABLESCAN_HPP


#include "Operator.hpp"
#include "SPSegment.hpp"
#include "Relation.hpp"

class TableScan : public Operator {
private:
    SPSegment* sps;
    Relation* rel;
    TID t;
    TID current;
public:
    TableScan( Relation* rel );
    
    void open();
    bool next();
    std::vector<Register> getOutput();
    void close();
    
};

#endif
