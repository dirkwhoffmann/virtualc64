// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* 
 * For a detailed description of the various file formats, see
 * http://www.infinite-loop.at/Power20/Documentation/Power20-LiesMich/AE-Dateiformate.html
 */

#ifndef _ANY_ARCHIVE_H
#define _ANY_ARCHIVE_H

#include "AnyCollection.h"

/* This class adds an API to AnyC64File for handling file formats that store a
 * collection of multiple C64 files (archives).
 */
class AnyArchive : public AnyCollection {
    
protected:
        
    // An offset into the data range of the selected item
    long iFp = -1;
    
    // End of file position (equals the last valid offset plus 1)
    long iEof = -1;
    
    
    //
    // Constructing
    //
    
public:
    
    /* Factory method. The method returns a pointer to a T64File, D64File,
     * PRGFile, P00File, or G64File object, depending on the type of the
     * specified file.
     */
    static AnyArchive *makeWithFile(const char *filename);
    
    AnyArchive() : AnyCollection() { }
    AnyArchive(size_t capacity) : AnyCollection(capacity) { }
    
    const char *getDescription() override { return "AnyArchive"; }


    //
    // Accessing items
    //
    
    // Returns the number of items in this archive
    virtual int numberOfItems() { return 0; }
    
    // Selects the active item (all item related methods work on this item)
    virtual void selectItem(unsigned item) { };
        
    // Returns a string representation of the item type ("PRG" etc.)
    virtual const char *getTypeOfItem() { return ""; }
    
    // Returns the name of the selected item
    virtual const char *getNameOfItem() { return ""; }
    virtual struct FSName getFSNameOfItem();

    // Returns the name of the selected item as a unicode character array
    const unsigned short *getUnicodeNameOfItem();
    

    //
    // Reading item data
    //
    
    // Returns the size of an item in bytes
    virtual size_t getSizeOfItem();
    
    // Returns the size of an item in blocks
    virtual size_t getSizeOfItemInBlocks() { return (getSizeOfItem() + 253) / 254; }

    // Returns an item in a buffer. The caller has to free the buffer.
    virtual void getItem(u8 **buf, size_t *cnt); 
    
    /* Moves the file pointer to the specified offset. seekItem(0) returns to
     * the beginning of the selected item.
     */
    virtual void seekItem(long offset) { };
    
    // Reads a byte from the selected item (-1 = EOF)
    virtual int readItem();
    
    // Reads multiple bytes (1 to 85) in form of a hex dump string
    virtual const char *readItemHex(size_t num);
    
    /* Returns the proposed memory location of the selected item. When a file
     * is flashed into memory, the item data is copied to this location.
     */
    virtual u16 getDestinationAddrOfItem() { return 0; }
    
    // Flashes the selected item into memory (buffer = pointer to C64 RAM)
    void flashItem(u8 *buffer);
    
 
    //
    // Debugging
    //
    
    virtual void dumpDirectory(); 
    
};

#endif

