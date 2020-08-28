// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _D64FILE_H
#define _D64FILE_H

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
    
    // Number of the currently selected item (-1 if no item is selected)
    long selectedItem = -1;
    
    
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
    static D64File *makeWithAnyArchive(AnyArchive *otherArchive);
    static D64File *makeWithDisk(Disk *disk);


    //
    // Initializing
    //
    
    D64File();
    D64File(unsigned tracks, bool ecc);
    
        
    //
    // Methods from AnyC64File
    //
    
    C64FileType type() override { return D64_FILE; }
    const char *typeAsString() override { return "D64"; }
    const char *getName() override;
    bool hasSameType(const char *filename) override { return isD64File(filename); }
    bool readFromBuffer(const u8 *buffer, size_t length) override;
    
    
    //
    // Methods from AnyArchive
    //
    
    int numberOfItems() override;
    void selectItem(unsigned n) override;
    const char *getTypeOfItem() override;
    const char *getNameOfItem() override;
    size_t getSizeOfItem() override;
    size_t getSizeOfItemInBlocks() override;
    void seekItem(long offset) override;
    int readItem() override;
    u16 getDestinationAddrOfItem() override;
    
    
    //
    // Methods from AnyDisk
    //
    
    int numberOfHalftracks() override;
    void selectHalftrack(Halftrack ht) override;
    size_t getSizeOfHalftrack() override;
    void seekHalftrack(long offset) override;

    void selectTrackAndSector(Track t, Sector s);

    
    //
    // Accessing file attributes
    //
    
    // Returns the first and second disk ID character
    u8 diskId1() { return data[offset(18, 0) + 0xA2]; }
    u8 diskId2() { return data[offset(18, 0) + 0xA3]; }
    
    
    //
    // Accessing file items
    //
    
private:
    
    /* Returns the offset to the first data byte of an item, or -1 if the item
     * does not exist.
     */
    long findItem(long item);
    
    /* Returns true iff item is a visible file. Whether a file is visible or
     * not is determined by the type character, a special byte stored inside
     * the directory. The type character also determines how the file is
     * displayed when the directory is loaded via LOAD "$",8. E.g., standard
     * program files are listes as PRG. If the optional argument is provided,
     * an extension string is returned (e.g. "PRG"). Invisible files return "".
     */
    bool itemIsVisible(u8 typeChar, const char **extension = NULL);
    
    
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
    
    // Returns true iff offset points to the last byte of a sector
    bool isLastByteOfSector(long offset) { return ((offset+1) % 256) == 0; }
    
    // Returns the next logical track number following this sector
    int nextTrack(long offset) { return data[offset & (~0xFF)]; }
    
    // Returns the next sector number following this sector
    int nextSector(long offset) { return data[(offset & (~0xFF)) + 1]; }
    
    // Returns the next physical track and sector
    bool nextTrackAndSector(Track track, Sector sector,
                            Track *nextTrack, Sector *nextSector,
                            bool skipDirectory = true);
    
    /* Jumps to the beginning of the next sector. The beginning of the next
     * sector is written into variable 'pos'. Returns true if the jump to the
     * next sector was successfull and false if the current sector points to an
     * invalid valid track/sector combination. In case of failure, pos remains
     * unchanged.
     */
    bool jumpToNextSector(long *pos);
    
    /* Writes a byte to the specified track and sector. If the sector overflows,
     * the values of track and sector are overwritten with the next free sector.
     * Returns true if the byte was written successfully and false if there is
     * no space left on disk.
     */
    bool writeByteToSector(u8 byte, Track *track, Sector *sector);
    
    
    //
    // Accessing file and directory items
    //
    
private:
    
    // Marks a single sector as "used"
    void markSectorAsUsed(Track track, Sector sector);
    
    // Writes the Block Availability Map (BAM) on track 18, sector 0
    void writeBAM(const char *name);
    
    /* Gathers data about all directory items. This function scans all
     * directory items and stores the relative start address of the first
     * sector into the provided offsets array. Furthermore, the total number
     * of files is written into variable noOfFiles. If the additional parameter
     * skipInvisibleFiles is set to true, only those files are considered that
     * would show up when loading the directory via LOAD "$",8. Otherwise, all
     * files are considered, i.e. those that are marked as deleted.
     */
    void scanDirectory(long *offsets, unsigned *noOfFiles, bool skipInvisibleFiles = true);
    
    /* Looks up a directory item by number. This function searches the
     * directory for the requested item. If the additional parameter
     * skipInvisibleFiles is set to true, only those files are considered that
     * would show up when loading the directory via LOAD "$",8. Otherwise, all
     * files are considered, i.e. those that are marked as deleted. The
     * function returns an offset to the first data sector of the requested
     * file. If the file is not found, -1 is returned.
     */
    long findDirectoryEntry(long item, bool skipInvisibleFiles = true);
    
    /* Returns the track or sector number of the first file block.
     * Example usages: firstTrackOfFile(findDirectoryEntry(42))
     *                 firstSectorOfFile(findDirectoryEntry(42))
     */
    u8 firstTrackOfFile(unsigned dirEntry) { return data[dirEntry + 1]; }
    u8 firstSectorOfFile(unsigned dirEntry) { return data[dirEntry + 2]; }
    
    // Returns true if offset points to the last byte of a file
    bool isEndOfFile(long offset) {
        return nextTrack(offset) == 0 && nextSector(offset) == offset % 256; }
    
    /* Writes a directory item. This function is used to convert other archive
     * formats into the D64 format.
     */
    bool writeDirectoryEntry(unsigned nr, const char *name,
                             Track startTrack, Sector startSector,
                             size_t filesize);
    
    
    //
    // Debugging
    //
    
private:
    
    // Dumps the contents of a sector to stderr
    void dump(Track track, Sector sector);
};
#endif
