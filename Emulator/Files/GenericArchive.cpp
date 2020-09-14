// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "GenericArchive.h"

GenericArchive::GenericArchive(const char *archiveName)
{
    strncpy(name, archiveName, sizeof(name));
    name[sizeof(name) - 1] = 0;
    ascii2petStr(name);
}

int
GenericArchive::numberOfItems()
{
    return (int)items.size();
}

void
GenericArchive::selectItem(unsigned n)
{
    selectedItem = n < numberOfItems() ? n : -1;
    offset = 0;
}

const char *
GenericArchive::getTypeOfItem()
{
    return selectedItem == -1 ? "" : CBMFileTypeString(items[selectedItem].type);
}

const char *
GenericArchive::getNameOfItem()
{
    return selectedItem == -1 ? "" : items[selectedItem].name;
}

size_t
GenericArchive::getSizeOfItem()
{
    return selectedItem == -1 ? 0 : items[selectedItem].size;
}

int
GenericArchive::readItem()
{
    if (selectedItem == -1 || offset == items[selectedItem].size) return -1;
    return items[selectedItem].data[offset++];
}

void
GenericArchive::seekItem(long offset)
{
    if (selectedItem == -1) {
        this->offset = 0;
        return;
    }
    
    if (offset >= 0 && offset < items[selectedItem].size) {
        this->offset = offset;
    }
}

u16
GenericArchive::getDestinationAddrOfItem()
{
    return 0x0801;
}
 
bool
GenericArchive::add(const char *name, CBMFileType type, u8 *data, size_t size)
{
    return add(name, type, data, size, items.size());
}
 
bool
GenericArchive::add(const char *name, CBMFileType type, u8 *data, size_t size, long at)
{
    GenericItem newItem;
    
    // TODO: Add error checks here
    // Limit the number of items to 144
    // Limit the size of a single item to <MAX C64 FILE SIZE>
    
    // Generate a PETSCII name
    strncpy(newItem.name, name, 16);
    newItem.name[16] = 0;
    ascii2petStr(newItem.name);
    
    // Copy type, size and data
    newItem.type = type;
    newItem.size = size;
    newItem.data = new u8[size];
    memcpy(newItem.data, data, size);
    
    items.insert(items.begin() + at, newItem);
    return true;
}
 
bool
GenericArchive::remove(long at)
{
    // Only proceed if the provided index is valid
    if (at < 0 || at >= items.size()) return false;
    
    items.erase(items.begin() + at);
    return true;
}

bool
GenericArchive::swap(long at1, long at2)
{
    // Only proceed if the provided indices are valid
    if (at1 < 0 || at1 >= items.size()) return false;
    if (at2 < 0 || at2 >= items.size()) return false;
    
    if (at1 != at2) {
        iter_swap(items.begin() + at1, items.begin() + at2);
    }
    return true;
}
