/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

#ifndef _FILEARCHIVE_INC
#define _FILEARCHIVE_INC

#include "Archive.h"

/*! @class FileArchive
 *  @brief The FileArchive class declares the programmatic interface for a file that does not match any of the standard formats.
    @discussion If a file does not match any of the standard formats, each byte is interpreted as raw data and is loaded at the standard memory location.
 */
class FileArchive : public Archive {

private:
	//! Raw file data
	uint8_t *data;

	//! Name buffer
	char name[18];

	//! File pointer (offset into data array)
	int fp;
		
	//! Size of data array
	int size;

public:

	//! Constructor
	FileArchive();
	
	//! Destructor
	~FileArchive();
	
	//! Returns true if filename points to a valid file of that type
	static bool isAcceptableFile(const char *filename);

	//! Factory method
	static FileArchive *archiveFromFile(const char *filename);

#pragma mark Container
	
	bool fileIsValid(const char *filename);
	bool readFromBuffer(const uint8_t *buffer, unsigned length);
	void dealloc();
    ContainerType getType();
    const char *getTypeAsString();
	
#pragma mark Archive

	int getNumberOfItems();
	const char *getNameOfItem(int n);
	const char *getTypeOfItem(int n);
	int getSizeOfItem(int n);
	uint16_t getDestinationAddrOfItem(int n);	
	void selectItem(int n);
	int getByte();
};
#endif
