#ifndef _PRGARCHIVE_INC
#define _PRGARCHIVE_INC

#include "Archive.h"

class PRGArchive : public Archive {

private:
	//! Physical name of archive 
	char *path;
	
	//! Name of the PRG container file
	char name[256];

	//! Raw data of PRG container file
	uint8_t *data;

	//! File pointer
	/*! Stores an offset into the data array */
	int fp;
		
	//! Size of data array
	int size;

public:

	//! Constructor
	PRGArchive::PRGArchive();
	
	//! Destructor
	PRGArchive::~PRGArchive();
	
	//! Check file type
	/*! Returns true, iff the specifies file is a valid archive file. */
	static bool fileIsValid(const char *filename);

	// Inherited from Archive class...

	char *getTypeOfArchive() { return "PRG container"; }
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
