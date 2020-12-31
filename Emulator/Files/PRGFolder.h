// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PRG_FOLDER_H
#define _PRG_FOLDER_H

#include "AnyArchive.h"

using std::vector;

class PRGFolder : public AnyArchive {
    
    typedef struct {
        
        char name[17];
        size_t size;
        u8 *data;
        u16 loadAddr;
        
    } Item;
    
    // Storage
    vector <Item> items;
    
    // Number of the currently selected item (-1 if no item is selected)
    long selectedItem = -1;
        
public:

    //
    // Class methods
    //
        
    // Returns true iff the specified path points to a PRG folder
    static bool isPRGFolder(const char *path);
    
    
    //
    // Constructing
    //
    
    static PRGFolder *makeWithFolder(const char *path);
    static PRGFolder *makeWithAnyArchive(AnyArchive *otherArchive);

    
    //
    // Initializing
    //
    
    PRGFolder() { };
    const char *getDescription() override { return "PRGFolder"; }
    
    
    //
    // Methods from AnyFile
    //

    FileType type() override { return FILETYPE_PRG_FOLDER; }
    bool hasSameType(const char *path) override { return isPRGFolder(path); }
    
    
    //
    // Methods from AnyArchive (DEPRECATED)
    //
    
    int numberOfItems() override;
    void selectItem(unsigned item) override;
    const char *getTypeOfItem() override { return "PRG"; }
    const char *getNameOfItem() override;
    size_t getSizeOfItem() override;
    void seekItem(long offset) override;
    u16 getDestinationAddrOfItem() override;

    
    //
    // Managing items
    //
    
    void add(PRGFile *prg);
    void add(PRGFile *prg, long at);
    bool remove(long at);
    bool swap(long at1, long at2);
};

#endif
