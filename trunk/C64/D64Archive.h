/*
 * Author: Dirk W. Hoffmann, www.dirkwhoffmann.de
 *
 * This program is free software; you can redistribute it and/or modify
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

#ifndef _D64ARCHIVE_INC
#define _D64ARCHIVE_INC

#include "Archive.h"

// Forward declarations
class VC1541;

// D64 files come in six different sizes
#define D64_683_SECTORS 174848
#define D64_683_SECTORS_ECC 175531
#define D64_768_SECTORS 196608
#define D64_768_SECTORS_ECC 197376
#define D64_802_SECTORS 205312
#define D64_802_SECTORS_ECC 206114

/*! @class Container
 @brief Base class for all loadable objects. */

/*! @class D64Archive
    @brief An archive of type D64. */
class D64Archive : public Archive {

//protected:
private: 

	//! @brief The logical name of this archive.
    char name[256];

	//! @brief The raw data of this archive.
	uint8_t data[D64_802_SECTORS_ECC];
	
	//! @brief Error information stored in the D64 archive.
	uint8_t errors[802];
	
	/*! @brief The number of tracks stored in this archive.
        @discussion Possible values are 35, 40, and 42. */
	unsigned numTracks; 
	
	/*! @brief File pointer
        @discussion An offset into the data array. */
	int fp;

private:
    
	//! Translate track and sector numbers
	int offset(int track, int sector);

	//! Returns true iff offset points to the last byte of a sector
	bool isLastByteOfSector(int offset) { return ((offset+1) % 256) == 0; }

	//! Returns true iff offset points to the last byte of a file
	bool isEndOfFile(int offset);
	
	//! Returns the next logical track number following this sector
	int nextTrack(int offset) { return data[(offset / 256) * 256]; }

	//! Returns the next logical sector number following this sector
	int nextSector(int offset) { return data[((offset / 256) * 256)+1]; }

	//! Return the next physical track and sector
	bool nextTrackAndSector(uint8_t track, uint8_t sector, uint8_t *nextTrack, uint8_t *nextSector, bool skipDirectory = false);

	//! Return the next physical track and sector and skip track 18 (directory track)
	// void nextTrackAndSectorSkipDirectory(uint8_t track, uint8_t sector, uint8_t *nextTrack, uint8_t *nextSector);
	
	//! Return beginning of next sector 
	int jumpToNextSector(int pos); 
	
	void dumpDir();
	int findDirectoryEntry(int itemNumber);

	//! Write BAM (track 18, sector 0)
	void writeBAM(const char *name);
	
	//! Write directory item (used to convert other archive format into D64 format)
	bool writeDirectoryEntry(unsigned nr, const char *name, uint8_t startTrack, uint8_t startSector, unsigned filesize);

	//! Write byte to specified track and sector
	/*! If sector overflows, the values of track and sector are overwritten with the next free sector */
	bool writeByteToSector(uint8_t byte, uint8_t *track, uint8_t *sector);

public:

    //
    //! @functiongroup Creating and destructing D64 archives
    //

    //! @brief Standard constructor.
	D64Archive();
    
    //! @brief Standard destructor.
	~D64Archive();
    
    /*! @brief Creates a D64 archive from a D64 file located on disc.
     *  @param filename The location of a file in D64 format.
     *  @return A newly created D64 archive; NULL, if the file does not exist or has a different format.
     */
    static D64Archive *archiveFromD64File(const char *filename);

	/*! @brief Create a D64 archive from a file located on disc.
	 * @discussion If the provided filename points to a D64 archive, @link archiveFromD64File @/link ist invoked. If the provided filename points to a file with different format (e.g. T64, PRG), the file format is converted to the D64 format on the fly.
     *  @param filename The location of a file in one of the supported file formats.
     */
     static D64Archive *archiveFromArbitraryFile(const char *filename);

    /*! @brief Creates a D64 archive from another D64 archive.
     *  @param archive The source archive to read from.
     *  @result A one to one copy of the source archive.
     *  @seealso @link archiveFromOtherArchive @/link */
    static D64Archive *archiveFromD64Archive(D64Archive *archive);

    /*! @brief Creates a D64 archive from an arbitrary archive.
     *  @param archive The source archive to read from.
     *  @result A D64 archive that contains the same files as the source archive.
     *  @seealso @link archiveFromD64Archive @/link. */
    static D64Archive *archiveFromArchive(Archive *archive);

    /*! @brief Creates a D64 archive from drive contents.
     *  @param drive A VC1541 drive with a disk inserted.
     *  @result A D64 archive containing the same files as the currently inserted disk; NULL if no disk is inserted. */
     static D64Archive *archiveFromDrive(VC1541 *drive);

    
	//
    // Virtual functions from Container class
    //
    
	bool fileIsValid(const char *filename);
	bool readFromBuffer(const uint8_t *buffer, unsigned length);
    unsigned writeToBuffer(uint8_t *buffer);
    void dealloc() { };
    ContainerType getType() { return D64_CONTAINER; }
    const char *getTypeAsString() { return "D64"; }
	const char *getName();
	
    
    //
	// Virtual functions from Archive class
	//
    
    int getNumberOfItems();
	const char *getNameOfItem(int n);
	const char *getTypeOfItem(int n);
	int getSizeOfItem(int n);
	uint16_t getDestinationAddrOfItem(int n);	
	void selectItem(int n);
	int getByte();

    
    //
    //! @functiongroup Accessing archive attributes
    //
    
    //! Returns true of filename points to a valid file of that type
    static bool isD64File(const char *filename);
    
    //! Class function that returns the number of sectors in a specific track
    static unsigned numberOfSectors(unsigned trackNr);

	//! Returns the number of tracks stored in this image
	unsigned numberOfTracks();
	
	//! Return pointer to raw sector data
	uint8_t *findSector(unsigned track, unsigned sector);
	
	//! Return LO BYTE of disk ID
	uint8_t diskIdLow() { return data[offset(18, 1) + 0xA2]; }

	//! Return HI BYTE of disk ID
	uint8_t diskIdHi() { return data[offset(18, 1) + 0xA3]; }

 
    //
    //! @functiongroup Debugging
    //
    
	//! Dump sector contents to stderr
	void dumpSector(int track, int sector);
	
    //! @functiongroup Misc
	
	//! Mark as single sector as "used"
	void markSectorAsUsed(uint8_t track, uint8_t sector);
		
};
#endif
