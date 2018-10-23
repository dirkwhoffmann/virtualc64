/*!
 * @header      D64File.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _D64FILE_INC
#define _D64FILE_INC

#include "AnyDisk.h"
#include "Disk.h"

// Forward declarations
// class Disk;

// D64 files come in six different sizes
#define D64_683_SECTORS 174848
#define D64_683_SECTORS_ECC 175531
#define D64_768_SECTORS 196608
#define D64_768_SECTORS_ECC 197376
#define D64_802_SECTORS 205312
#define D64_802_SECTORS_ECC 206114


/*! @class   D64File
 *  @brief   The D64File class declares the programmatic interface for a file
 *           in D64 format.
 */
class D64File : public AnyDisk {

private: 
    
    /*! @brief    Number of the currently selected halftrack
     *  @details  0, if no halftrack is selected
     */
    Halftrack selectedHalftrack  = 0;
    
	/*! @brief   Error information stored in the D64 archive.
     */
	uint8_t errors[802];
	
	/*! @brief   The number of tracks stored in this archive.
        @details Possible values are 35, 40, and 42.
     */
	// unsigned numTracks; 
	
    /*! @brief    Number of the currently selected item
     *  @details  -1, if no item is selected
     */
    long selectedItem = -1;
    
public:

    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a D64 file
    static bool isD64Buffer(const uint8_t *buffer, size_t length);
    
    //! @brief   Returns true iff the specified file is a D64 file.
    static bool isD64File(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing objects
    //

    //! @brief    Standard constructor
    D64File();

    //! @brief    Custom constructor
    D64File(unsigned tracks, bool ecc);
    
    //! @brief    Factory method
    static D64File *makeObjectWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static D64File *makeObjectWithFile(const char *path);
    
    //! @brief    Factory method
    static D64File *makeObjectWithAnyArchive(AnyArchive *otherArchive);
    
    //! @brief    Factory method
    static D64File *makeObjectWithDisk(Disk *disk);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    C64FileType type() { return D64_FILE; }
    const char *typeAsString() { return "D64"; }
    const char *getName();
    bool hasSameType(const char *filename) { return isD64File(filename); }
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    
    
    //
    //! @functiongroup Methods from AnyArchive
    //
    
    int numberOfItems();
    void selectItem(unsigned n);
    const char *getTypeOfItemAsString();
    const char *getNameOfItem();
    size_t getSizeOfItem();
    size_t getSizeOfItemInBlocks();
    void seekItem(long offset);
    int readItem();
    uint16_t getDestinationAddrOfItem();
    
private:
    
    /*! @brief    Returns the offset to the first data byte of an item.
     *  @return   -1, if the item does not exist.
     */
    long findItem(long item);
    
public:
    
    
    //
    // Methods from AnyDisk
    //
    
    int numberOfHalftracks();
    void selectHalftrack(Halftrack ht);
    size_t getSizeOfHalftrack();
    void seekHalftrack(long offset);
    
    
    
    
    //! @brief    Returns a pointer to the raw archive data
    //! @deprecated
    uint8_t *getData() { return data; }

    //! @brief    Returns the number of tracks stored in this image
    //! @deprecated
    // unsigned numberOfTracks();
    
    
    /*! @brief    Returns true iff item is a visible file
     *  @details  Whether a file is visible or not is determined by the type character,
     *            a special byte stored inside the directory. The type character also
     *            determines how the file is displayed when the directory is loaded via
     *            LOAD "$",8. E.g., standard program files are listes as PRG.
     *  @param    typeChar   The type character of a file.
     *  @param    extension  If this argument is provided, an extension string is
     *            returned (e.g. "PRG"). Invisible files return "" as extension string.
     */
    bool itemIsVisible(uint8_t typeChar, const char **extension = NULL);
    
    //! @brief    Class function that returns the total number of sectors in a specific track
    // static unsigned numberOfSectors(unsigned trackNr);

	//! @brief    Returns the first disk ID character
	uint8_t diskId1() { return data[offset(18, 0) + 0xA2]; }

	//! @brief    Returns the second disk ID character
	uint8_t diskId2() { return data[offset(18, 0) + 0xA3]; }

 
    //
    //! @functiongroup Accessing tracks and sectors
    //
    
public:

    //! @brief    Returns a pointer to the raw sector data.
    uint8_t *findSector(Track track, Sector sector);

    //! @brief    Returns the error for the specified sector.
    /*! @note     Returns 01 (no error) if the D64 file does not contain
     *            error codes.
     */
    uint8_t errorCode(Track t, Sector s);
    
private:
        
    //! @brief   Translates a track and sector number into an offset.
    //! @return  -1, if an invalid track or sector number is provided.
    int offset(Track track, Sector sector);
    
    //! @brief   Returns true iff offset points to the last byte of a sector.
    bool isLastByteOfSector(long offset) { return ((offset+1) % 256) == 0; }
    
    //! @brief   Returns the next logical track number following this sector.
    /*! @note    The number is stored in the first byte of the current sector.
     */
    int nextTrack(long offset) { return data[offset & (~0xFF)]; }
    
    //! @brief   Returns the next sector number following this sector.
    /*! @note    The number is stored in the second byte of the current sector.
     */
    int nextSector(long offset) { return data[(offset & (~0xFF)) + 1]; }
    
    //! @brief   Returns the next physical track and sector.
    bool nextTrackAndSector(Track track, Sector sector,
                            Track *nextTrack, Sector *nextSector,
                            bool skipDirectory = true);
    
    /*! @brief   Jump to the beginning of the next sector
     *  @details pos is set to the beginning of the next sector.
     *  @result  True if the jump to the next sector was successful; false if
     *           the current sector points to an invalid valid track/sector
     *           combination. In the failure case, pos remains untouched.
     */
    bool jumpToNextSector(long *pos);

    /*! @brief   Writes a byte to the specified track and sector
     *  @details If the sector overflows, the values of track and sector are
     *           overwritten with the next free sector.
     *  @result  true if the byte was written successfully; false if there is
     *           no space left on disk.
     */
    bool writeByteToSector(uint8_t byte, Track *track, Sector *sector);

    
    //
    //! @functiongroup Accessing file and directory items
    //

private:
    
    /*! @brief   Marks a single sector as "used"
     */
    void markSectorAsUsed(Track track, Sector sector);

    /*! @brief   Writes the Block Availability Map (BAM)
     *  @details On a C64 diskette, the BAM is located ion track 18, sector 0.
     *  @param   name Name of the disk
     */
    void writeBAM(const char *name);

    /*! @brief   Gathers data about all directory items
     *  @details This function scans all directory items and stores the relative
     *           start address of the first sector into the provided offsets
     *           array. Furthermore, the total number of files is written into
     *           variable noOfFiles.
     *  @param   offsets Pointer to an array of type unsigned[MAX_FILES_ON_DISK]
     *  @param   noOfFiles Pointer to a variable of type unsigned
     *  @param   skipInvisibleFiles If set to true, only those files are
     *           considered that would show up when loading the directory via
     *           LOAD "$",8. Otherwise, all files are considered, i.e. those
     *           that are marked as deleted.
     */
    void scanDirectory(long *offsets, unsigned *noOfFiles, bool skipInvisibleFiles = true);
    
    /*! @brief   Looks up a directory item by number.
     *  @details This function searches the directory for the requested item. 
     *  @param   itemBumber Number of the item. The first item has number 0.
     *  @param   skipInvisibleFiles If set to true, only those files are
     *           considered that would show up when loading the directory via
     *           LOAD "$",8. Otherwise, all files are considered, i.e. those
     *           that are marked as deleted.
     *  @return  Offset to the first data sector of the requested file. If the
     *           file is not found, -1 is returned.
     */
    long findDirectoryEntry(long item, bool skipInvisibleFiles = true);
    
    //! Returns the track number of the first file block
    /*! Example usage: firstTrackOfFile(findDirectoryEntry(42)) */
    uint8_t firstTrackOfFile(unsigned dirEntry) { return data[dirEntry + 1]; }

    //! @brief    Returns the sector number of the first file block
    /*! @details  Example usage: firstSectorOfFile(findDirectoryEntry(42)) 
     */
    uint8_t firstSectorOfFile(unsigned dirEntry) { return data[dirEntry + 2]; }
    
    /*! @brief    Returns true iff offset points to the last byte of a file 
     */
    bool isEndOfFile(long offset) {
        return nextTrack(offset) == 0 && nextSector(offset) == offset % 256; }

    /*! @brief    Writes a directory item
     *  @details  This function is used to convert other archive formats into
     *            the D64 format.
     */
    bool writeDirectoryEntry(unsigned nr, const char *name,
                             Track startTrack, Sector startSector,
                             size_t filesize);
    

    //
    //! @functiongroup Debugging
    //
    
private:
    
	//! @brief    Dumps the contents of a sector to stderr
	void dumpSector(Track track, Sector sector);
};
#endif
