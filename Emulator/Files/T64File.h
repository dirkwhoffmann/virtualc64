// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _T64_FILE_H
#define _T64_FILE_H

#include "AnyCollection.h"

class T64File : public AnyCollection {
    
    // Header signature
    static const u8 magicBytes[];
    
    // Number of the currently selected item (-1 if no item is selected)
    long selectedItem = -1;
    
public:
    
    //
    // Class methods
    //
    
    // Returns true iff buffer contains a T64 file
    static bool isT64Buffer(const u8 *buffer, size_t length);
    
    // Returns true of filename points to a valid file of that type
    static bool isT64File(const char *filename);
    
    
    //
    // Creating
    //
    
    static T64File *makeWithBuffer(const u8 *buffer, size_t length);
    static T64File *makeWithFile(const char *path);
    static T64File *makeWithFileSystem(class FSDevice *fs);

    
    //
    // Initializing
    //
    
    T64File() : AnyCollection() { }
    T64File(size_t capacity) : AnyCollection(capacity) { }

    const char *getDescription() override { return "T64File"; }

    //
    // Methods from AnyFile
    //
    
    FileType type() override { return FileType_T64; }
    const char *getName() override;
    bool matchingBuffer(const u8 *buf, size_t len) override;
    bool matchingFile(const char *path) override;
    bool oldReadFromBuffer(const u8 *buffer, size_t length) override;
    
    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    u64 collectionCount() override;
    PETName<16> itemName(unsigned nr) override;
    u64 itemSize(unsigned nr) override;
    u8 readByte(unsigned nr, u64 pos) override;
    
private:

    u16 memStart(unsigned nr);
    u16 memEnd(unsigned nr);
    
    
    //
    // Scanning and repairing a T64 file
    //
    
public:
    
    // Checks if the header contains information at the specified location
    bool directoryItemIsPresent(int n);

    /* Checks the file for inconsistencies and tries to repair it. This method
     * can eliminate the following inconsistencies:
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
     *
     * Returns true if archive was consistent or could be repaired. Returns
     * false if an inconsistency has been detected that could not be repaired.
     */
    bool repair();
};

#endif

