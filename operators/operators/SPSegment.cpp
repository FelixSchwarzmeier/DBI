#include "SPSegment.hpp"

#include "SlottedPage.hpp"

#include <iostream>

uint64_t SPSegment::createPageId() {
        return pageIdCounter++;
}
    

//Passende Seite finden, wird beim insert benötigt
SlottedPage& SPSegment::getFittingPage(unsigned len) {
    //Suche nach passender Page
    for( auto& sp : spm ) {
        //Page genug Platz?
        Header* hdr = (Header*) sp.second;
        if( hdr->freeSpace >= len ) {
            return *sp.second;
        }
        //Page genug Platz bei rearangement?
        else if ( hdr->freeSpace + hdr->spaceByArrangement >= len ) {
            sp.second->arrangePage();
            return *sp.second;
        }
        
    }
    //Keine freie Page gefunden, es wird eine neue geholt;
    uint64_t pI = createPageId();
    spm[pI] = new SlottedPage(bm, pI);
    return *spm[pI];
}
    
    
    //depreciated?
    /*getSlot(BufferManager& bm){
        BufferFrame& bf = bm.fix(pageId, false);
        char* data = bf.getdata();
    }*/

    SPSegment::SPSegment( BufferManager& bm) : bm(bm) {
        pageIdCounter = 0;
    }

    
    //Insert a Record
    TID SPSegment::insert(const Record& r) {
        std::cout << "Insert SPSegment" << std::endl;
        SlottedPage& sp = getFittingPage(r.getLen());
        unsigned slotId = sp.insert(r);
        return TID(sp.getPageId(), slotId);
    }
    
    //Stupid Solution
    bool SPSegment::remove(TID tid) {
        //TID Verweis finden und dann drauf arbeiten
        TID run = spm[tid.pageId]->getDest(tid);
        while( spm[run.pageId]->final(run) ) {
            run = spm[run.pageId]->getDest(run);
        }
        
        return spm[run.pageId]->remove(run);
    }
    
    Record SPSegment::lookup(TID tid) {
        //TID Verweis finden und dann drauf arbeiten
        TID run = spm[tid.pageId]->getDest(tid);
        while( spm[run.pageId]->final(run) ) {
            run = spm[run.pageId]->getDest(run);
        }
        
        return spm[run.pageId]->lookup(run);
    }
    
    //TBD Verweisverfolgung bos gefunden
    bool SPSegment::update(TID tid, const Record& r) {
        //Einfuegen
        TID newTID = insert(r);
        //TID Verweis finden und dann drauf arbeiten
        TID run = spm[tid.pageId]->getDest(tid);
        while( spm[run.pageId]->final(run) ) {
            run = spm[run.pageId]->getDest(run);
        }
        
        //CreateVerweis
        spm[run.pageId]->createVerweis(run, newTID);
        
        return true;
    }
    
    uint64_t SPSegment::get_pageIdCounter() {
        return pageIdCounter;
    }
    
    unsigned SPSegment::get_slotIdCounter( uint64_t pageid) {
        return spm[pageid]->get_numSlots();
    }