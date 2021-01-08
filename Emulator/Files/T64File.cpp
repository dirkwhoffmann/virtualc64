// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "TAPFile.h"
#include "T64File.h"
#include "FSDevice.h"

/* "Anmerkung: Der String muß nicht wortwörtlich so vorhanden sein. Man sollte nach den
 *  Substrings 'C64' und 'tape' suchen." [Power64 doc]
 */
// const u8 T64File::magicBytes[] = { 0x43, 0x36, 0x34 };

bool
T64File::isCompatibleName(const std::string &name)
{
    auto s = suffix(name);
    return s == "t64" || s == "T64";
}

bool
T64File::isCompatibleStream(std::istream &stream)
{
    const u8 magicBytes[] = { 0x43, 0x36, 0x34 };
    
    if (streamLength(stream) < 0x40) return false;
    return matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

T64File *
T64File::makeWithFileSystem(class FSDevice &fs)
{
    debug(FILE_DEBUG, "Creating T64 archive...\n");
    
    // Analyze the file system
    u16 numFiles = (u16)fs.numFiles();
    std::vector<u64> length(numFiles);
    usize dataLength = 0;
    for (u16 i = 0; i < numFiles; i++) {
        length[i] = fs.fileSize(i) - 2;
        dataLength += length[i];
    }
    
    // Create new archive
    u16 maxFiles = MAX(numFiles, 30);
    usize fileSize = 64 + maxFiles * 32 + dataLength;
    T64File *t64 = new T64File(fileSize);
    
    //
    // Header
    //
    
    // Magic bytes (32 bytes)
    u8 *ptr = t64->data;
    strncpy((char *)ptr, "C64 tape image file", 32);
    ptr += 32;
    
    // Version (2 bytes)
    *ptr++ = 0x01;
    *ptr++ = 0x01;
    
    // Max files (2 bytes)
    *ptr++ = LO_BYTE(maxFiles);
    *ptr++ = HI_BYTE(maxFiles);
    
    // Stored files (2 bytes)
    *ptr++ = LO_BYTE(numFiles);
    *ptr++ = HI_BYTE(numFiles);
    
    // Reserved (2 bytes)
    *ptr++ = 0x00;
    *ptr++ = 0x00;
    
    // User description (24 bytes, padded with 0x20)
    auto name = PETName<24>(fs.getName().c_str(), 0x20);
    name.write(ptr);
    ptr += 24;
    
    assert(ptr - t64->data == 64);
    
    //
    // Tape entries
    //
    
    u32 tapePosition = 64 + maxFiles * 32; // Start of item 0
    memset(ptr, 0, 32 * maxFiles);
    
    for (unsigned n = 0; n < maxFiles; n++) {
        
        // Skip if this is an empty tape slot
        if (n >= numFiles) { ptr += 32; continue; }
                        
        // Entry used (1 byte)
        *ptr++ = 0x01;
        
        // File type (1 byte)
        *ptr++ = 0x82;
        
        // Start address (2 bytes)
        u16 startAddr = fs.loadAddr(n);
        *ptr++ = LO_BYTE(startAddr);
        *ptr++ = HI_BYTE(startAddr);
        
        // End address (2 bytes)
        u16 endAddr = startAddr + length[n];
        *ptr++ = LO_BYTE(endAddr);
        *ptr++ = HI_BYTE(endAddr);
        
        // Reserved (2 bytes)
        ptr += 2;
        
        // Tape position (4 bytes)
        *ptr++ = LO_BYTE(tapePosition);
        *ptr++ = LO_BYTE(tapePosition >> 8);
        *ptr++ = LO_BYTE(tapePosition >> 16);
        *ptr++ = LO_BYTE(tapePosition >> 24);
        tapePosition += fileSize;
        
        // Reserved (4 bytes)
        ptr += 4;
        
        // File name (16 bytes)
        PETName<16> name = fs.fileName(n);
        name.write(ptr);
        ptr += 16;
    }
    
    //
    // File data
    //
    
    for (unsigned n = 0; n < numFiles; n++) {
    
        fs.copyFile(n, ptr, length[n], 2);
        ptr += length[n];
    }

    debug(FILE_DEBUG, "T64 file created");

    return t64;
}

PETName<16>
T64File::getName()
{
    return PETName<16>(data + 0x28, 0x20);
}

PETName<16>
T64File::collectionName()
{
    return PETName<16>(data + 0x28, 0x20);
}

u64
T64File::collectionCount()
{
    return LO_HI(data[0x24], data[0x25]);
}

PETName<16>
T64File::itemName(unsigned nr)
{
    assert(nr < collectionCount());
    
    u8 padChar = 0x20;
    return PETName<16>(data + 0x50 + nr * 0x20, padChar);
    /*
    string result = "";

    unsigned first = 0x50 + (nr * 0x20);
    unsigned last  = 0x60 + (nr * 0x20);
    
    for (unsigned i = first; i < last; i++) {
        result += (char)(data[i] == 0x20 ? ' ' : data[i]);
    }

    return name;
    */
}

u64
T64File::itemSize(unsigned nr)
{
    assert(nr < collectionCount());
    
    // Return the number of data bytes plus 2 (for the loading address header)
    return memEnd(nr) - memStart(nr) + 2;
}

u8
T64File::readByte(unsigned nr, u64 pos)
{
    assert(nr < collectionCount());
    assert(pos < itemSize(nr));

    // The first two bytes are the loading address which is stored seperately
    if (pos <= 1) return pos ? HI_BYTE(memStart(nr)) : LO_BYTE(memStart(nr));
    
    // Locate the first byte of the requested file
    unsigned i = 0x48 + (nr * 0x20);
    u64 start = LO_LO_HI_HI(data[i], data[i+1], data[i+2], data[i+3]);

    // Locate the requested byte
    u64 offset = start + pos - 2;
    assert(offset < size);
    
    return data[offset];
}

u16
T64File::memStart(unsigned nr)
{
    return LO_HI(data[0x42 + nr * 0x20], data[0x43 + nr * 0x20]);
}

u16
T64File::memEnd(unsigned nr)
{
    return LO_HI(data[0x44 + nr * 0x20], data[0x45 + nr * 0x20]);
}

bool
T64File::directoryItemIsPresent(int item)
{
    int first = 0x40 + (item * 0x20);
    int last  = 0x60 + (item * 0x20);
    int i;
    
    // check for zeros...
    if (last < (long)size)
        for (i = first; i < last; i++)
            if (data[i] != 0)
                return true;
    
    return false;
}

void
T64File::repair()
{
    unsigned i, n;
    u16 noOfItems = collectionCount();

    //
    // 1. Repair number of items, if this value is zero
    //
    
    if (noOfItems == 0) {

        while (directoryItemIsPresent(noOfItems))
            noOfItems++;

        u16 noOfItemsStatedInHeader = collectionCount();
        if (noOfItems != noOfItemsStatedInHeader) {
            
            warn("T64: Changing number of items from %d to %d.\n",
                  noOfItemsStatedInHeader, noOfItems);
            
            data[0x24] = LO_BYTE(noOfItems);
            data[0x25] = HI_BYTE(noOfItems);
            
        }
        assert(noOfItems == collectionCount());
    }
    
    for (i = 0; i < noOfItems; i++) {

        //
        // 2. Check relative offset information for each item
        //

        // Compute start address in file
        n = 0x48 + (i * 0x20);
        u16 startAddrInContainer = LO_LO_HI_HI(data[n], data[n+1], data[n+2], data[n+3]);

        if (startAddrInContainer >= size) {
            warn("T64: Offset mismatch. Sorry, can't repair.\n");
            return;
        }
    
        //
        // 3. Check for file end address mismatches (as created by CONVC64)
        //
        
        // Compute start address in memory
        n = 0x42 + (i * 0x20);
        u16 startAddrInMemory = LO_HI(data[n], data[n+1]);
    
        // Compute end address in memory
        n = 0x44 + (i * 0x20);
        u16 endAddrInMemory = LO_HI(data[n], data[n+1]);
    
        if (endAddrInMemory == 0xC3C6) {

            // Let's assume that the rest of the file data belongs to this file ...
            u16 fixedEndAddrInMemory = startAddrInMemory + (size - startAddrInContainer);

            warn("T64: Changing end address of item %d from %04X to %04X.\n",
                 i, endAddrInMemory, fixedEndAddrInMemory);

            data[n] = LO_BYTE(fixedEndAddrInMemory);
            data[n+1] = HI_BYTE(fixedEndAddrInMemory);
        }
    }
}
