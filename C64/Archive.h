/*!
 * @header      Archive.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 */
/*
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
/* 
 * For a detailed description of the various file formats, see
 * http://www.infinite-loop.at/Power20/Documentation/Power20-LiesMich/AE-Dateiformate.html
 */

#ifndef _ARCHIVE_INC
#define _ARCHIVE_INC

#include "File.h"

/*! @class    Archive
    @brief    Base class for all loadable objects with multiple files included. */

class Archive : public File {
    
public:

    //
    //! Creating and destructing containers
    //
    
    //! @brief    Standard constructor
	Archive();
    
    //! @brief    Standard destructor
	virtual ~Archive();
    
    //
    //! Factory methods
    //
    
    static Archive *makeArchiveWithFile(const char *filename);
    
    //
    //! Accessing archive attributes
    //

    //! @brief    Returns the number of items in this archive.
    virtual int getNumberOfItems() { return 0; }

    
    //
    //! Accessing item attributes
    //
			
    /*! @brief   Returns the name of an item
     *  @return  String in ASCII format, or NULL, if item does not exist
     */
    virtual const char *getNameOfItem(int n) { return NULL; }

    /*! @brief   Returns the name of an item
     *  @return  Unicode character array, or NULL, if item does not exist
     *  @details The provides unicode format is compatible with font C64ProMono
     *           which is used, e.g., in the mount dialogs preview panel. 
     */
    virtual const unsigned short *getUnicodeNameOfItem(int n, size_t maxChars = 255) { return NULL; }

    //! @brief    Returns the type of an item as a string (e.g., "PRG" or "DEL")
    virtual const char *getTypeOfItem(int n) { return NULL; }
	
    //! @brief    Returns the size of an item in bytes
	virtual size_t getSizeOfItem(int n);

    //! @brief    Returns the size of an item in bits
    virtual size_t getSizeOfItemInBits(int n) { return 8 * getSizeOfItem(n); }

    //! @brief    Returns the size of an item in blocks
	virtual size_t getSizeOfItemInBlocks(int n) { return (getSizeOfItem(n) + 253) / 254; }
		
    /*! @brief    Returns the proposed memory location of an item.
     *  @details  When a file is flashed into memory, the raw data is copied to this location.
     */
    virtual uint16_t getDestinationAddrOfItem(int n) { return 0; }
	
    
    //
    //! @functiongroup Reading an item
    //

    //! @brief    Selects an item to read from
    virtual void selectItem(int n) { }
	
    //! @brief    Reads the next byte from the currently selected item
    virtual int getByte() { return 0; }

    //! @brief    Skip n bytes from the currently selected item
    virtual void skip(unsigned n) { for (unsigned i = 0; i < n; i++) (void)getByte(); }

    //! @brief    Reads multiply bytes in form of string
    const char *byteStream(unsigned n, size_t offset, size_t num);
    
    //
    //! @functiongroup Debugging
    //
    
    virtual void dumpDirectory(); 
    
};

#endif

