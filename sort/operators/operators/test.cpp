

#include "test.hpp"

#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>


#include "DBMS.hpp"


int main(int argc, char* argv[]) {
    BufferManager bm(100);
    SPSegment sps(bm);
    Relation rel(&sps);
    rel.addColumn(false, 4);//zahl 4 byte
    rel.addColumn(true, 12);// cstring 12 byte
    
    
    TableScan TS(&rel);
    //tbd
    
    return 0;
}
