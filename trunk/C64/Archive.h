#ifndef _ARCHIVE_INC
#define _ARCHIVE_INC

#include "basic.h"

class Archive {

	//! Will store the directory of this archive
	//* The directory is stored in the same format that would be sent from a floppy drive */
	char *dir;
	
public:

	//! Constructor
	Archive();
	
	//! Destructor
	~Archive();

	//! Search directory for filename and return item number
	/*! Returns -1, if the file is not found.
		The function supports the wildcard characters '?' and '*' */
	int getItemWithName(char *filename);

	//! Is archive mountable?
	/*! Returns true, iff the archive can be mounted as a disk drive */
	virtual bool isMountable(void) { return false; }

	//! Is archive flashable?
	/*! Returns true, iff the archive can be flashed into memory */
	virtual bool isFlashable(void) { return true; }
		
	//! Type of archive (T64, D64, PRG, ...)
	virtual char *getTypeOfArchive() = 0;

	//! Load pyhsical archive from disc
	virtual bool loadFile(const char *filename) = 0;

	//! Discard previously loaded contents
	virtual void eject() = 0;

	//! Physical name of archive on disc
	virtual char *getPath() = 0;
	
	//! Logical name of archive
	virtual char *getName() = 0;
	
	//! Number of stored items
	virtual int getNumberOfItems() = 0;
	
	//! Get name of n-th item
	virtual char *getNameOfItem(int n) = 0;

	//! Get file type of n-th item
	virtual char *getTypeOfItem(int n) = 0;
	
	//! Get size of n-th item in bytes
	virtual int getSizeOfItem(int n) = 0;

	//! Get size of n-th item in blocks
	int getSizeOfItemInBlocks(int n) { return (getSizeOfItem(n) + 253) / 254; }
		
	//! Get destination memory location
	/*! When flash() is invoked, the raw data is copied to this location in virtual memory. */
	virtual uint16_t getDestinationAddrOfItem(int n) = 0;
	
	//! Select item to read from
	/*! You need to select an item before you read data */
	virtual void selectItem(int n) = 0;
	
	//! Read next byte from selected item
	/*! -1 indicates EOF (End of File) */
	virtual int getByte() = 0;	
};

#endif

