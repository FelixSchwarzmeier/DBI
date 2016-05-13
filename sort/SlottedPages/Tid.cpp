#include "Tid.hpp"

TID::TID(unsigned pageId, unsigned slotId) : pageId(pageId), slotId(slotId) {
}

unsigned TID::getPageId() {
    return pageId;
}
unsigned TID::getSlotId() {
    return pageId;
}