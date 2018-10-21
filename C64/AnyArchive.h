/*!
 * @header      AnyArchive.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
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
/* 
 * For a detailed description of the various file formats, see
 * http://www.infinite-loop.at/Power20/Documentation/Power20-LiesMich/AE-Dateiformate.html
 */

#ifndef _ARCHIVE_INC
#define _ARCHIVE_INC

#include "AnyC64File.h"

/*! @class    AnyArchive
 *  @brief    Base class for all file types that allow a direct access to all
 *            stored files.
 */

class AnyArchive : public AnyC64File {
    
public:

    //
    //! @functiongroup Creating and deleting objects
    //
    
    static AnyArchive *makeArchiveWithFile(const char *filename);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    /*! @brief    Flashes the selected item into memory
     *  @param    buffer must be a pointer to the C64 RAM
     *  @seealso  getDestinationAddr() which is used to determine the target
     *            address in RAM.
     */
    void flash(uint8_t *buffer);
    
    
    //
    //! @functiongroup Accessing archive attributes
    //

    //! @brief    Returns the number of items in this archive.
    virtual int numberOfItems() { return 0; }

    /*! @brief   Returns the name of an item in ASCII format.
     *  @note    Never call this function for nonexisting items.
     */
    virtual const char *getNameOfItem(unsigned n);

    /*! @brief   Returns the name of an item in unichar format.
     *  @details The returned unichars are compatible with font C64ProMono
     *           which is used, e.g., in the mount dialogs preview panel.
     *  @note    Never call this function for nonexisting items.
     */
    virtual const unsigned short *getUnicodeNameOfItem(unsigned n);

    //! @brief    Returns the type of an item as a string (e.g., "PRG" or "DEL")
    virtual const char *getTypeOfItem(unsigned n) { return NULL; }
    
    //! @brief    Returns the size of an item in bytes
    virtual size_t getSizeOfItem(unsigned n);

    //! @brief    Returns the size of an item in blocks
    virtual size_t getSizeOfItemInBlocks(unsigned n) { return (getSizeOfItem(n) + 253) / 254; }
        
    /*! @brief    Returns the proposed memory location of an item.
     *  @details  When a file is flashed into memory, the raw data is copied to
     *            this location.
     */
    virtual uint16_t getDestinationAddrOfItem(unsigned n) { return 0; }

    /*! @brief    Returns the proposed memory location of the selected item.
     *  @details  When a file is flashed into memory, the item data is copied
     *            to this location.
     */
    virtual uint16_t getDestinationAddr() { return 0; }
    
    //
    //! @functiongroup Reading an item
    //

    //! @brief    Selects an item to read from
    virtual void selectItem(unsigned n) { };
    
    //! @brief    Reads multiple bytes in form of string
    // const char *hexDump(unsigned n, size_t offset, size_t num);
    
    
    //
    //! @functiongroup Debugging
    //
    
    virtual void dumpDirectory(); 
    
};

#endif

