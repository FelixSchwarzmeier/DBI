#include "SlottedPage.hpp"

#include "Tid.hpp"
#include "BufferManager.hpp"

#include <cstdlib>
#include <iostream>

SlottedPage::SlottedPage( BufferManager& bm, uint64_t pageId) : bm(bm), pageId(pageId) {     
    newPage = true;
}
    
/*SlottedPage::SlottedPage() {
    bm = BufferManager(100);
    std::cout << "SlottedPage Constrcutor called ohne Argumente" << std::endl;
}*/
    
    
uint64_t SlottedPage::getPageId() {
    return pageId;
}
//Removing all the Spaces from a Page
void SlottedPage::arrangePage() {
    BufferFrame bf = bm.fixPage(pageId, false); //vll. true?
    char* data = (char*)bf.getData();
    
    int moveBack = 0;
    for( int i = 0; i < freeSpaces.size(); i++ ) {
        memcpy(data + freeSpaces[i].offset + freeSpaces[i].length - moveBack, data + freeSpaces[i].offset - moveBack, 8*1024-freeSpaces[i].offset);
        moveBack += freeSpaces[i].length;
    }
    
    freeSpaces = std::vector<Slot>();
    
    bm.unfixPage(bf, true);
}
    
    
unsigned SlottedPage::insert( const Record& rec ) {
    std::cout << "Insert SlottedPage" << std::endl;
    BufferFrame bf = bm.fixPage(pageId, false);//vll true?
    char* data = (char*)bf.getData();
    
    Header* hdr = (Header*)data;
    
    //Leere Page, initialisierung der Werte
    if( /*!hdr->slotCount*/ newPage == true ) {
        std::cout << "EMPTY PAGE INITIALISED" << std::endl;
        newPage = false;
        hdr->slotCount = 0;
        hdr->firstFreeSlot = sizeof(Header);
        hdr->dataStart = 8*1024;
        hdr->freeSpace = 8*1024-sizeof(Header)-sizeof(Slot);
        hdr->spaceByArrangement = 0;
    }
    
    std::cout << "passed empty page" << std::endl;
    
    
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
    
    std::cout << "Before memcpy" << std::endl;
    std::cout << data << std::endl;
    std::cout << slot->offset << std::endl;
    std::cout << data + slot->offset << std::endl;
    std::cout << rec.getData() << std::endl;
    std::cout << rec.getLen() << std::endl; 
    std::cout << "out" << std::endl;
    //CopyData
    memcpy(data+slot->offset, rec.getData(), rec.getLen());
    
    std::cout << "Aftermemcpy memcpy" << std::endl;
    
    bm.unfixPage(bf, true);
    
    return hdr->firstFreeSlot;
}

//Needs to be improved
bool SlottedPage::remove(TID tid) {
    BufferFrame bf = bm.fixPage(tid.pageId, false); //vll. true?
    char* data = (char*)bf.getData();
    
    Header* hdr = (Header*)data;
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
    hdr->spaceByArrangement = s->length;
    freeSpaces.push_back(*s);//Save free part so rearrange page later on
    s->offset = 0;
    s->length = 0;
    bm.unfixPage(bf, true);
    
    return true;
}

Record SlottedPage::lookup( TID tid ) {
    BufferFrame bf = bm.fixPage(tid.pageId, false);
    char* data = (char*)bf.getData();
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
    
    
    Record* ret = (Record*)(data+s->offset);
    
    Record* rec = static_cast<Record*>(malloc(ret->getLen()));
    memcpy(rec, ret, ret->getLen());
    
    
    
    
    bm.unfixPage(bf, false);
    return Record(rec->getLen(), rec->getData());
}


TID SlottedPage::getDest(TID tid) {
    BufferFrame bf = bm.fixPage(tid.pageId, false);
    char* data = (char*)bf.getData();
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
    //Verweise
    TID ret(0,0);
    if( s->length & 0x80000000) {
        ret = TID(s->offset, s->length & 0x7fffffff);
    //Gefunden
    } else {
        ret = tid;
    }
    
    bm.unfixPage(bf, false);
    return ret;
}

bool SlottedPage::final(TID tid) {
    BufferFrame bf = bm.fixPage(tid.pageId, false);
    char* data = (char*)bf.getData();
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
    
    bool ret = true;
    //FInal?
    if( s->length & 0x80000000 ){
        return false;
    }
    
    bm.unfixPage(bf, false);
    return ret;
}

bool SlottedPage::createVerweis(TID tid, TID newTID ) {
    BufferFrame bf = bm.fixPage(tid.pageId, false);
    char* data = (char*)bf.getData();
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
    //Verweise erzeugen
    s->offset = newTID.pageId;
    s->length = newTID.slotId | 0x80000000;
    
    bm.unfixPage(bf, true);
    return true;
}

unsigned SlottedPage::get_numSlots() {
    return slotCount;
}