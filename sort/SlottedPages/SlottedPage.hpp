#ifndef H_SlottedPage_HPP
#define H_SlottedPage_HPP


class SlottedPage {
    
    
    
    unsigned pageId;
    
    //Header
    
    class Header {
        unsigned slotCount;
        unsigned firstFreeSlot;
        unsigned dataStart;
        unsigned freeSpace;
    }
    
    //Slots
    class Slot {
        unsigned offset;
        unsigned length;
    }
    
    //Records
    
    
    
    SlottedPage( BufferManager& bm, unsigned pageId) : bm(bm), pageId(pageId) {        
    };
    
    
    unsigned insert( BufferManager& bm, Record& rec ) {
        BufferFrame bf& = bm.fixPage(pageId, false);//vll true?
        char* data = bf.getData();
        
        Header* hdr = (Header*)data;
        
        //Leere Page, initialisierung der Werte
        if( !hdr->slotCount ) {
            hdr->firstFreeSlot = data+sizeof(Header);
            hdr->dataStart = 300;
            hdr->freeSpace = 300-sizeof(Header);//TBD
        }
        
        
        //Slot management
        (Slot*) slot = hdr->firstFreeSlot;
        hdr->slotCount++;
        ((Slot*)hdr->firstFreeSlot)++;
    
        //Header Management
        hdr->dataStart -= rec.getLen();
        hdr->freeSpace -= rec.getLen();
        
        //Set Slot
        slot->offset = hdr->dataStart;
        slot->length = rec.getLen();
        
        //CopyData
        memcpy(data+slot->offset, rec.getData(); rec.getLen(););
        
        bm.unfix(pageId, true)
    }
    
    //Needs to be improved
    bool remove(TID tid) {
        BufferFrame bf& = bm.fixPage(tid.getPageid, false); //vll. true?
        char* data = bf.getData();
        
        Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
        s.offset = 0;
        s.length = 0;
        return true;
    }
    
    Record& lookup( BufferManager& bm, TID tid ) {
        BufferFrame bf& = bm.fixPage(tid.getPageid, false);
        char* data = bf.getData();
        
        Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
        //Noch nicht ganz safe, erst kopieren, dann friegeben!
        bm.unfix(pageId, false);
        return *((Record*)(data+s->offset));
    }
    
    
    
    
};



#endif