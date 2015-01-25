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

#ifndef _ARCHIVE_INC
#define _ARCHIVE_INC

#include "Container.h"

/*!
 * @class Container
 * @brief Base class for all loadable objects with multiple files included
 */
class Archive : public Container {
	
public:

	Archive();
	virtual ~Archive();
	
    /*! @brief Write protection flag.
     *  @discussion Indicates whether the archives represents a write protected medium.
     *  The flag determines the state of the optical write protection sensor of the VC1541 drive when an archive is inserted as disk.
     *  @deprecated The flag is only meaningful for D64 archives and should be implemented there.
     */
    bool writeProtection;
    
    //! @brief The number of items in this archive.
    virtual int getNumberOfItems() = 0;

    /*! @brief Search directory for a specific item.
     *  @param filename The name of a directory item. The name may contain the wildcard characters '?' and '*'.
     *  @return The number of item or -1, if no matching item was found. The first item is numbered 0.
     */
	int getItemWithName(char *filename);
			
    /*! @brief Returns the name of the item located at the specified index.
     *  @return NULL, if the item does not exists.
     */
	virtual const char *getNameOfItem(int n) = 0;

    /*! @brief Returns the type of the item located at the specified index.
     *  @return The type of the item as a string, e.g., "PRG" or "DEL".
     */
	virtual const char *getTypeOfItem(int n) = 0;
	
    /*! @brief Returns the size of the item located at the specified index.
     *  @return The size of item \param n in bytes. Returns 0, if the item does not exist.
     */
	virtual int getSizeOfItem(int n) = 0;

    /*! @brief Returns the size of the item located at the specified index.
     *  @return The size of the item in blocks. Returns 0, if the item does not exist.
     */
	int getSizeOfItemInBlocks(int n) { return (getSizeOfItem(n) + 253) / 254; }
		
    /*! @brief Returns the memory location of an item.
     *  @discussion When a file is flashed into memory, the raw data is copied to this location.
     *  @return The destination address in the C64 ram.
     */
	virtual uint16_t getDestinationAddrOfItem(int n) = 0;
	

    /*! @brief Select item to read from
     *  @discussion This functions has to be invoked before calling \see getBytes
     */
	virtual void selectItem(int n) = 0;
	
    /*! @brief Read next byte from the currently selected item
     *  @return The next character from the currently selected item or -1 (indicating EOF)
     *  @seealso getByte
     */
	virtual int getByte() = 0;
};

#endif

