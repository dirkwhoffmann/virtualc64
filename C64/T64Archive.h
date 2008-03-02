#ifndef _T64ARCHIVE_INC
#define _T64ARCHIVE_INC

#include "Archive.h"

class T64Archive : public Archive {

private:
	//! Physical name of archive 
	char *path;
	
	//! Name of the T64 container file
	char name[256];

	//! Raw data of T64 container file
	uint8_t *data;

	//! File pointer
	/*! Stores an offset into the data array */
	int fp;
	
	//! End of file position
	int fp_eof;
	
	//! Size of data array
	uint32_t size;

	//! Returns true, iff the archive contains the n-th directory item
	//* Set n to zero to watch for the first item */
	bool directoryItemIsPresent(int n);

public:

	//! Constructor
	T64Archive::T64Archive();
	
	//! Destructor
	T64Archive::~T64Archive();
	
	//! Check file type
	/*! Returns true, iff the specifies file is a valid archive file. */
	static bool fileIsValid(const char *filename);

	// Inherited from Archive class...

	char *getTypeOfArchive() { return "T64 container"; }
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
};
#endif

