#include "SlottedPage.cpp"

#include "Tid.hpp"
#include "BufferManager.hpp"

SlottedPage( BufferManager& bm, unsigned pageId) : bm(bm), pageId(pageId) {        
    };
    
    
unsigned SlottedPage::insert( BufferManager& bm, Record& rec ) {
    BufferFrame bf = bm.fixPage(pageId, false);//vll true?
    void* data = bf.getData();
    
    Header* hdr = (Header*)data;
    
    //Leere Page, initialisierung der Werte
    if( !hdr->slotCount ) {
        hdr->firstFreeSlot = sizeof(Header);
        hdr->dataStart = 8*1024;
        hdr->freeSpace = 8*1024-sizeof(Header);
    }
    
    
    //Slot management
    Slot* slot = (Slot*)(data + hdr->firstFreeSlot);
    hdr->slotCount++;
    hdr->firstFreeSlot += sizeof(Slot);

    //Header Management
    hdr->dataStart -= rec.getLen();
    hdr->freeSpace -= rec.getLen();
    
    //Set Slot
    slot->offset = hdr->dataStart;
    slot->length = rec.getLen();
    
    //CopyData
    memcpy(data+slot->offset, rec.getData(), rec.getLen());
    
    bm.unfixPage(bf, true);
}

//Needs to be improved
bool SlottedPage::remove(TID tid) {
    BufferFrame bf = bm.fixPage(tid.getPageid, false); //vll. true?
    char* data = bf.getData();
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.getSlotId;
    s.offset = 0;
    s.length = 0;
    return true;
}

Record& SlottedPage::lookup( BufferManager& bm, TID tid ) {
    BufferFrame bf& = bm.fixPage(tid.getPageid, false);
    char* data = bf.getData();
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
    //Noch nicht ganz safe, erst kopieren, dann friegeben!
    bm.unfix(pageId, false);
    return *((Record*)(data+s->offset));
}