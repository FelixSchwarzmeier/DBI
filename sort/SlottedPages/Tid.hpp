#ifndef H_SPSegment_HPP
#define H_SPSegment_HPP



class TID {
    unsigned pageId;
    unsigned slotId;
    
    TID(unsigned pageId, unsigned slotId) : pageId(pageId), slotId(slotId) {
    }
    
    unsigned getPageId() {
        return pageId;
    }
    unsigned getSlotId() {
        return pageId;
    }
    
};



#endif