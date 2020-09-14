// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _GENERIC_ARCHIVE_H
#define _GENERIC_ARCHIVE_H

#include <vector>
#include "AnyArchive.h"

using std::vector;

typedef struct {
    
    char name[17];
    CBMFileType type;
    u8 *data;
    size_t size;
    
} GenericItem;


class GenericArchive : public AnyArchive {
            
    // Item storage
    vector <GenericItem> items;
    
    // Number of the currently selected item (-1 if no item is selected)
    long selectedItem = -1;
    
    long offset = 0;
    
public:
        
    //
    // Creating
    //
    
    static GenericArchive *makeWith(AnyArchive *otherArchive);

    
    //
    // Initializing
    //
    
    GenericArchive(const char *archiveName);
    
    
    //
    // Methods from AnyC64File
    //
    
    FileType type() override { return FILETYPE_GENERIC_ARCHIVE; }
    const char *typeAsString() override { return "GenericArchive"; }
    
    
    //
    // Methods from AnyArchive
    //
    
    int numberOfItems() override;
    void selectItem(unsigned n) override;
    const char *getTypeOfItem() override;
    const char *getNameOfItem() override;
    size_t getSizeOfItem() override;
    int readItem() override;
    void seekItem(long offset) override;
    u16 getDestinationAddrOfItem() override;
    

    //
    // Managing items
    //
    
    bool add(const char *name, CBMFileType type, u8 *data, size_t size);
    bool add(const char *name, CBMFileType type, u8 *data, size_t size, long at);
    bool remove(long at);
    bool swap(long at1, long at2);
};

#endif
