// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _D64_FILE_H
#define _D64_FILE_H

#include "AnyDisk.h"
#include "Disk.h"

// D64 files come in six different sizes
#define D64_683_SECTORS 174848
#define D64_683_SECTORS_ECC 175531
#define D64_768_SECTORS 196608
#define D64_768_SECTORS_ECC 197376
#define D64_802_SECTORS 205312
#define D64_802_SECTORS_ECC 206114

class D64File : public AnyDisk {
    
    // Number of the currently selected track (0 if no track is selected)
    Halftrack selectedHalftrack = 0;
    
    // Error information stored in the D64 archive
    u8 errors[802];
    
    
    //
    // Class methods
    //
    
public:
    
    // Returns true iff buffer contains a D64 file
    static bool isD64Buffer(const u8 *buffer, size_t length);
    
    // Returns true iff the specified file is a D64 file
    static bool isD64File(const char *filename);
    
    
    //
    // Constructing
    //
    
    static D64File *makeWithBuffer(const u8 *buffer, size_t length);
    static D64File *makeWithFile(const char *path);
    static D64File *makeWithDisk(Disk *disk);
    static D64File *makeWithDrive(Drive *drive);
    static D64File *makeWithVolume(class FSDevice &volume, FSError *err);


    //
    // Initializing
    //
    
    D64File();
    D64File(unsigned tracks, bool ecc);
    const char *getDescription() override { return "D64File"; }

        
    //
    // Methods from AnyC64File
    //
    
    FileType type() override { return FileType_D64; }
    const char *getName() override;
    bool hasSameType(const char *filename) override { return isD64File(filename); }
    bool readFromBuffer(const u8 *buffer, size_t length) override;
    
    
    //
    // Methods from AnyDisk
    //
    
    int numberOfHalftracks() override;
    void selectHalftrack(Halftrack ht) override;
    size_t getSizeOfHalftrack() override;
    void seekHalftrack(long offset) override;

    void selectTrackAndSector(Track t, Sector s);


    //
    // Accessing disk attributes
    //

    PETName<16>getPETName();

    
    //
    // Accessing file attributes
    //
    
    // Returns the first and second disk ID character
    u8 diskId1() { return data[offset(18, 0) + 0xA2]; }
    u8 diskId2() { return data[offset(18, 0) + 0xA3]; }
    
    
    //
    // Accessing tracks and sectors
    //
    
public:
    
    /* Returns the error for the specified sector, or 01 (no error) if the D64
     * file does not contain error codes.
     */
    u8 errorCode(Track t, Sector s);
    
private:
    
    /* Translates a track and sector number into an offset. Returns -1, if an
     * invalid track or sector number is provided.
     */
    int offset(Track track, Sector sector);
    
    
    
    //
    // Debugging
    //
    
private:
    
    // Dumps the contents of a sector
    void dump(Track track, Sector sector);
};

#endif
