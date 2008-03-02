#ifndef _D64ARCHIVE_INC
#define _D64ARCHIVE_INC

#include "Archive.h"

class D64Archive : public Archive {

private:
	//! Physical name of archive 
	char *path;
	
	//! Name of the D64 container file
	char name[256];

	//! Raw data of D64 container file
	uint8_t data[206114];
	
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
	
	//! Returns the next track number following this sector
	int nextTrack(int offset) { return data[(offset / 256) * 256]; }

	//! Returns the next sector number following this sector
	int nextSector(int offset) { return data[((offset / 256) * 256)+1]; }

	//! Return beginning of next sector 
	int jumpToNextSector(int pos); 
	
	//! Returns true, iff the archive contains the n-th directory item
	//* Set n to zero to watch for the first item */
	//bool directoryItemIsPresent(int n);

	void dumpDir();
	int findDirectoryEntry(int itemNumber);


public:

	//! Constructor
	D64Archive::D64Archive();
	
	//! Destructor
	D64Archive::~D64Archive();
	
	//! Check file type
	/*! Returns true, iff the specifies file is a valid archive file. */
	static bool fileIsValid(const char *filename);

	// Inherited from Archive class...

	char *getTypeOfArchive() { return "D64 container"; }
	bool loadFile(const char *filename);
	void eject();
	char *getPath();
	char *getName();
	int getNumberOfItems();
	char *getNameOfItem(int n);
	char *getTypeOfItem(int n);
	int getSizeOfItem(int n);
	uint16_t getDestinationAddrOfItem(int n);	
	void selectItem(int n);
	int getByte();

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

};
#endif
