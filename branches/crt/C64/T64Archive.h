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
	int size;

	//! Returns true, iff the archive contains the n-th directory item
	//* Set n to zero to watch for the first item */
	bool directoryItemIsPresent(int n);

public:

	//! Constructor
	T64Archive();
	
	//! Destructor
	~T64Archive();
	
	//! Check file type
	/*! Returns true, iff the specifies file is a valid archive file. */
	static bool fileIsValid(const char *filename);

	// Inherited from Archive class...

	const char *getTypeOfArchive() { return "T64 container"; }
	bool loadFile(const char *filename);
	void eject();
	const char *getPath();
	const char *getName();
	int getNumberOfItems();
	const char *getNameOfItem(int n);
	const char *getTypeOfItem(int n);
	int getSizeOfItem(int n);
	uint16_t getDestinationAddrOfItem(int n);	
	void selectItem(int n);
	int getByte();
};
#endif

