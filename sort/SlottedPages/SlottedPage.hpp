#ifndef H_SlottedPage_HPP
#define H_SlottedPage_HPP

#include "Tid.hpp"
#include "BufferManager.hpp"

class SlottedPage {
    
    
    
    unsigned pageId;
    BufferManager bm;
    
    //Header
    
    class Header {
        public:
            unsigned slotCount;
            unsigned firstFreeSlot;
            unsigned dataStart;
            unsigned freeSpace;
    };
    
    //Slots
    class Slot {
        public:
            unsigned offset;
            unsigned length;
    };
    
    //Records
    
    
    
    SlottedPage( BufferManager& bm, unsigned pageId);
    
    
    unsigned insert( BufferManager& bm, Record& rec );
    
    //Needs to be improved
    bool remove(TID tid);
    
    Record& lookup( BufferManager& bm, TID tid );
    
    
    
    
};



#endif