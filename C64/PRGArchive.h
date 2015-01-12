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

#ifndef _PRGARCHIVE_INC
#define _PRGARCHIVE_INC

#include "Archive.h"

class PRGArchive : public Archive {

private:
	//! Name of the PRG container file
	// char name[256];

	//! Raw data of PRG container file
	uint8_t *data;

	//! File pointer
	/*! Stores an offset into the data array */
	int fp;
		
	//! Size of data array
	int size;

public:

	//! Constructor
	PRGArchive();
	
	//! Destructor
	~PRGArchive();
	
	//! Returns true of filename points to a valid file of that type
	static bool isPRGFile(const char *filename);

	//! Factory method
	static PRGArchive *archiveFromFile(const char *filename);

	//! Virtual functions from Container class
	bool fileIsValid(const char *filename);
	bool readFromBuffer(const void *buffer, unsigned length);
	bool readDataFromFile(FILE *file, struct stat fileProperties);
	void dealloc();
    ContainerType getType();
	const char *getTypeAsString();
	
	// Virtual functions from Archive class
	int getNumberOfItems();
	const char *getNameOfItem(int n);
	const char *getTypeOfItem(int n);
	int getSizeOfItem(int n);
	uint16_t getDestinationAddrOfItem(int n);	
	void selectItem(int n);
	int getByte();
};
#endif
