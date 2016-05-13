#ifndef H_TID_HPP
#define H_TID_HPP



class TID {
    unsigned pageId;
    unsigned slotId;
    
    public:
        TID(unsigned pageId, unsigned slotId);
        
        unsigned getPageId();
        unsigned getSlotId();
};



#endif