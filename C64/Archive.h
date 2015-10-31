/*
 * Author: Dirk W. Hoffmann, www.dirkwhoffmann.de
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

// For a detailed description of the various file formats, see
// http://www.infinite-loop.at/Power20/Documentation/Power20-LiesMich/AE-Dateiformate.html


#ifndef _ARCHIVE_INC
#define _ARCHIVE_INC

#include "Container.h"

/*! @class Archive
    @brief Base class for all loadable objects with multiple files included. */

class Archive : public Container {
    
public:

    //
    //! Creating and destructing containers
    //
    
    //! Standard constructor.
	Archive();
    
    //! Standard destructor.
	virtual ~Archive();
    
    //
    //! Accessing archive attributes
    //

    //! Returns the number of items in this archive.
    virtual int getNumberOfItems() = 0;

    //
    //! Accessing item attributes
    //

    /*! Searches the directory for a specific item.
        @param filename The item name may contain the wildcard characters '?' and '*'.
        @return The number of the item (starting at 0) or -1, if no matching item was found. */
	int getItemWithName(char *filename);
			
    //! @brief Returns the name of an item (NULL, if the item does not exists)
	virtual const char *getNameOfItem(int n) = 0;

    //! @brief Returns the type of an item as a string (e.g., "PRG" or "DEL")
	virtual const char *getTypeOfItem(int n) = 0;
	
    //! @brief Returns the size of an item in bytes
	virtual int getSizeOfItem(int n);

    //! @brief Returns the size of an item in bits
    virtual int getSizeOfItemInBits(int n) { return 8 * getSizeOfItem(n); }

    //! @brief Returns the size of an item in blocks
	virtual int getSizeOfItemInBlocks(int n) { return (getSizeOfItem(n) + 253) / 254; }
		
    /*! @brief Returns the proposed memory location of an item.
        @discussion When a file is flashed into memory, the raw data is copied to this location. */
	virtual uint16_t getDestinationAddrOfItem(int n) = 0;
	
    //
    //! @functiongroup Reading an item
    //

    //! @brief Selects an item to read from
	virtual void selectItem(int n) = 0;
	
    //! @brief Reads the next byte from the currently selected item
	virtual int getByte() = 0;

    //
    //! @functiongroup Debugging
    //
    
    virtual void dumpDirectory();
    
};

#endif

