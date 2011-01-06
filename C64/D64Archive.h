/*
 * (C) 2007 Dirk W. Hoffmann. All rights reserved.
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

class D64Archive : public Archive {

private:
	//! Name of the D64 container file
	char name[256];

	//! Raw data of D64 container file
	uint8_t data[206114];
	
	//! Errors stored at the end of some D64 files
	uint8_t errors[768];
	
	//! Number of tracks in this image (can be 35, 40, or 42)
	unsigned numTracks; 
	
	//! File pointer
	/*! Stores an offset into the data array */
	int fp;
		
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

	//! Constructor
	D64Archive();
	
	//! Destructor
	~D64Archive();
	
	//! Returns true of filename points to a valid file of that type
	static bool isD64File(const char *filename);


	// Factory methods

	//! Create D64 archive from D64 file
	static D64Archive *archiveFromFile(const char *filename);

	//! Create D64 archive from arbitrary file
	/*! T64, PRG, P00, ... files are automatically converted to D64 format */
	static D64Archive *archiveFromArbitraryFile(const char *filename);
	
	//! Create D64 from other archive
	static D64Archive *archiveFromOtherArchive(Archive *archive);
	

	//! Virtual functions from Container class
	bool fileIsValid(const char *filename);
	bool readDataFromFile(FILE *file, struct stat fileProperties);
	void cleanup();
	const char *getTypeOfContainer();
	const char *getName();
	
	// Virtual functions from Archive class
	int getNumberOfItems();
	const char *getNameOfItem(int n);
	const char *getTypeOfItem(int n);
	int getSizeOfItem(int n);
	uint16_t getDestinationAddrOfItem(int n);	
	void selectItem(int n);
	int getByte();

	// Inherited from Archive class...
	bool isMountable() { return true; }
	bool isFlashable() { return true; }	

	//! Write to file
	bool writeToFile(const char *filename);
	
	//! Returns the number of sectors in the specified track
	unsigned numberOfSectors(unsigned trackNr);

	//! Returns the number of tracks stored in this image
	unsigned numberOfTracks();
	
	//! Return pointer to raw sector data
	uint8_t *findSector(unsigned track, unsigned sector);
	
	//! Return LO BYTE of disk ID
	uint8_t diskIdLow() { return data[offset(18, 1) + 0xA2]; }

	//! Return HI BYTE of disk ID
	uint8_t diskIdHi() { return data[offset(18, 1) + 0xA3]; }

	//! Dump sector contents to stderr
	void dumpSector(int track, int sector);
	
	//! Clear all data on disk
	void clear();
	
	//! Mark as single sector as "used"
	void markSectorAsUsed(uint8_t track, uint8_t sector);

	
	//! Write raw data to disk
	// bool writeData(uint8_t startTrack, uint8_t startSector, uint8_t *data, unsigned length, uint8_t *lastTrack, uint8_t *lastSector);
	
	//! Write archive contents to disk
	bool writeArchive(Archive *archive);
	
};
#endif
