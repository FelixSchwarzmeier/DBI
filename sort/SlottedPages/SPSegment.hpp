#ifndef H_SPSegment_HPP
#define H_SPSegment_HPP

#include "BufferManager.hpp"

#include "Tid.hpp"
#include "SlottedPage.hpp"



class SPSegment {
    
private:
    BufferManager bm;
    
    std::map<unsigned, SlottedPages> spm;
    
    unsigned pageIdCounter;
    unsigned createPageId() {
        return pageIdCounter++;
    }
    

    //Passende Seite finden, wird beim insert benötigt
    SlottedPages& getFittingPage(unsigned len) {
        //Suche nach passender Page
        for( SlottedPages& sp : spm ) {
            if( sp.freeSpace >= len ) {
                return sp;
            }
        }
        //Keine freie Page gefunden, es wird eine neue geholt;
        unsigned pI = createPageId();
        spm[pI] = SlottedPage(bm);
        return spv[pI];
    }
    
    
    //depreciated?
    getSlot(BufferManager& bm){
        BufferFrame& bf = bm.fix(pageId, false);
        char* data = bf.getdata();
    }

public:
    SPSegment( BufferManager& bm) : bm(bm) {
        pageIdCounter = 0;
    }

    
    //Insert a Record
    TID insert(const Record& r) {
        SlottedPage& sp = getFittingPage(r.getLen());
        unsigned slotId = sp.insert(r);
        return TID(sp.pageId, slotId);
    }
    
    //Stupid Solution
    bool SPSegment::remove(TID tid) {
        return spm[tid.pageId].remove(tid);
    }
    
    Record SPSegment::lookup(TID tid) {
        return spm[tid.pageId].lookup(tid);
    }
    
    //TBD
    bool SPSegment::update(TID tid, const Record& r) {
        //remove(tid);
        //insert()
        return true;
    }
    
    
};



#endif