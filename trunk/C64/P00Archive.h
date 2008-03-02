#ifndef _P00ARCHIVE_INC
#define _P00ARCHIVE_INC

#include "Archive.h"

class P00Archive : public Archive {

private:
	//! Physical name of archive 
	char *path;
	
	//! Name of the P00 container file
	char name[256];

	//! Raw data of P00 container file
	uint8_t *data;

	//! File pointer
	/*! Stores an offset into the data array */
	int fp;
		
	//! Size of data array
	uint32_t size;

public:

	//! Constructor
	P00Archive::P00Archive();
	
	//! Destructor
	P00Archive::~P00Archive();
	
	//! Check file type
	/*! Returns true, iff the specifies file is a valid archive file. */
	static bool fileIsValid(const char *filename);

	// Inherited from Archive class...

	char *getTypeOfArchive() { return "P00 container"; }
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
