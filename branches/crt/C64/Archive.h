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
	virtual ~Archive();

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
	virtual const char *getTypeOfArchive() = 0;

	//! Load pyhsical archive from disc
	virtual bool loadFile(const char *filename) = 0;

	//! Discard previously loaded contents
	virtual void eject() = 0;

	//! Physical name of archive on disc
	virtual const char *getPath() = 0;
	
	//! Logical name of archive
	virtual const char *getName() = 0;
	
	//! Number of stored items
	virtual int getNumberOfItems() = 0;
	
	//! Get name of n-th item
	virtual const char *getNameOfItem(int n) = 0;

	//! Get file type of n-th item
	virtual const char *getTypeOfItem(int n) = 0;
	
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

