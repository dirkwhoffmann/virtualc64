// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "T64File.h"
#include "FileSystem.h"
#include "IOUtils.h"
#include "Macros.h"

bool
T64File::isCompatible(const string &path)
{
    auto s = util::extractSuffix(path);
    return s == "t64" || s == "T64";
}

bool
T64File::isCompatible(std::istream &stream)
{
     const string magicT64 = "C64";
     const string magicTAP = "C64-TAPE";

     if (util::streamLength(stream) < 0x40) return false;
     
     // T64 files must begin with "C64"
     if (!util::matchingStreamHeader(stream, magicT64)) return false;
     
     // T64 files must *not* begin with "C64-TAPE" (which is used by TAP files)
     if (util::matchingStreamHeader(stream, magicTAP)) return false;
     
     return true;
}

void
T64File::init(class FileSystem &fs)
{
    // Analyze the file system
    isize numFiles = fs.numFiles();
    std::vector<isize> length(numFiles);
    isize dataLength = 0;
    
    for (isize i = 0; i < numFiles; i++) {
        
        length[i] = fs.fileSize(i) - 2;
        dataLength += length[i];
    }
    
    // Initialize the archive
    isize maxFiles = std::max(numFiles, (isize)30);
    isize fileSize = 64 + maxFiles * 32 + dataLength;
    init(fileSize);
    
    //
    // Header
    //
    
    // Magic bytes (32 bytes)
    u8 *ptr = data;
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
    
    assert(ptr - data == 64);
    
    //
    // Tape entries
    //
    
    isize tapePosition = 64 + maxFiles * 32; // Start of item 0
    memset(ptr, 0, 32 * maxFiles);
    
    for (isize n = 0; n < maxFiles; n++) {
        
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
        u16 endAddr = (u16)(startAddr + length[n]);
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
    
    for (isize n = 0; n < numFiles; n++) {
    
        fs.copyFile(n, ptr, length[n], 2);
        ptr += length[n];
    }
}

PETName<16>
T64File::getName() const
{
    return PETName<16>(data + 0x28).stripped(' ');
}

PETName<16>
T64File::collectionName()
{
    return PETName<16>(data + 0x28).stripped(' ');
}

isize
T64File::collectionCount() const
{
    return LO_HI(data[0x24], data[0x25]);
}

PETName<16>
T64File::itemName(isize nr) const
{
    assert(nr < collectionCount());
    
    return PETName<16>(data + 0x50 + nr * 0x20).stripped(' ');
}

u64
T64File::itemSize(isize nr) const
{
    assert(nr < collectionCount());
    
    // Return the number of data bytes plus 2 (for the loading address header)
    return (u64)(memEnd(nr) - memStart(nr) + 2);
}

u8
T64File::readByte(isize nr, u64 pos) const
{
    assert(nr < collectionCount());
    assert(pos < itemSize(nr));

    // The first two bytes are the loading address which is stored seperately
    if (pos <= 1) return pos ? HI_BYTE(memStart(nr)) : LO_BYTE(memStart(nr));
    
    // Locate the first byte of the requested file
    isize i = 0x48 + (nr * 0x20);
    u64 start = LO_LO_HI_HI(data[i], data[i+1], data[i+2], data[i+3]);

    // Locate the requested byte
    i64 offset = start + pos - 2;
    assert(offset < size);
    
    return data[offset];
}

u16
T64File::memStart(isize nr) const
{
    return LO_HI(data[0x42 + nr * 0x20], data[0x43 + nr * 0x20]);
}

u16
T64File::memEnd(isize nr) const
{
    return LO_HI(data[0x44 + nr * 0x20], data[0x45 + nr * 0x20]);
}

bool
T64File::directoryItemIsPresent(isize item)
{
    isize first = 0x40 + (item * 0x20);
    isize last  = 0x60 + (item * 0x20);
    isize i;
    
    // check for zeros...
    if (last < (isize)size)
        for (i = first; i < last; i++)
            if (data[i] != 0)
                return true;
    
    return false;
}

void
T64File::finalizeRead()
{
    /* This methods eliminates the following inconsistencies:
     *
     *   - number of files:
     *     Some archives state falsely in their header that zero files are
     *     present. This value will be fixed.
     *
     *   - end loading address:
     *     Archives that are created with CONVC64 often contain a value of
     *     0xC3C6, which is wrong (e.g., paradrd.t64). This value will be
     *     changed such that getByte() will read until the end of the physical
     *     file.
     */
    
    isize i, n;
    isize noOfItems = collectionCount();

    //
    // 1. Repair number of items, if this value is zero
    //
    
    if (noOfItems == 0) {

        while (directoryItemIsPresent(noOfItems))
            noOfItems++;

        isize noOfItemsStatedInHeader = collectionCount();
        if (noOfItems != noOfItemsStatedInHeader) {
            
            warn("T64: Changing number of items from %ld to %ld.\n",
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
        isize startAddrInContainer = LO_LO_HI_HI(data[n], data[n+1], data[n+2], data[n+3]);

        if (startAddrInContainer >= size) {
            warn("T64: Offset mismatch. Sorry, can't repair.\n");
            return;
        }
    
        //
        // 3. Check for file end address mismatches (as created by CONVC64)
        //
        
        // Compute start address in memory
        n = 0x42 + (i * 0x20);
        isize startAddrInMemory = LO_HI(data[n], data[n+1]);
    
        // Compute end address in memory
        n = 0x44 + (i * 0x20);
        isize endAddrInMemory = LO_HI(data[n], data[n+1]);
    
        if (endAddrInMemory == 0xC3C6) {

            // Let's assume that the rest of the file data belongs to this file ...
            isize fixedEndAddrInMemory = startAddrInMemory + (size - startAddrInContainer);

            warn("T64: Changing end address of item %ld from %04zX to %04zX.\n",
                 i, endAddrInMemory, fixedEndAddrInMemory);

            data[n] = LO_BYTE(fixedEndAddrInMemory);
            data[n+1] = HI_BYTE(fixedEndAddrInMemory);
        }
    }
}
