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
    //! @functiongroup Creating and destructing objects
    //
    
    /*! @brief    Factory method
     *  @return   A T64File, D64File, PRGFile, or P00File object, depending
     *            on the type of the specified file.
     */
    static AnyArchive *makeObjectWithFile(const char *filename);
    

    //
    //! @functiongroup Accessing items
    //

    //! @brief    Returns the number of items in this archive.
    virtual int numberOfItems() { return 0; }

    /*! @brief    Selects the active item
     *  @details  All item related methods work on the active item.
     */
    virtual void selectItem(unsigned n) { };
    
    //! @brief   Returns the name of the selected item in ASCII format.
    virtual const char *getNameOfItem() { return ""; }

    /*! @brief   Returns the name of the selected item in unichar format.
     *  @details The returned unichars are compatible with font C64ProMono
     *           which is used, e.g., in the mount dialogs preview panel.
     *  @note    Never call this function for nonexisting items.
     */
    virtual const unsigned short *getUnicodeNameOfItem();

    //! @brief    Returns the type of the selected in ASCII format (e.g., "PRG")
    virtual const char *getTypeOfItem() { return ""; }
    
    //! @brief    Returns the size of an item in bytes
    virtual size_t getSizeOfItem();

    //! @brief    Returns the size of an item in blocks
    virtual size_t getSizeOfItemInBlocks() { return (getSizeOfItem() + 253) / 254; }
        
    /*! @brief    Returns the proposed memory location of the selected item.
     *  @details  When a file is flashed into memory, the item data is copied
     *            to this location.
     *  @seealso  flashItem()
     */
    virtual uint16_t getDestinationAddrOfItem() { return 0; }
    
    /*! @brief    Flashes the selected item into memory.
     *  @param    buffer must be a pointer to the C64 RAM
     */
    void flashItem(uint8_t *buffer);
    
 
    //
    //! @functiongroup Debugging
    //
    
    virtual void dumpDirectory(); 
    
};

#endif

