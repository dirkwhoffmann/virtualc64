// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* 
 * For a detailed description of the various file formats, see
 * http://www.infinite-loop.at/Power20/Documentation/Power20-LiesMich/AE-Dateiformate.html
 */

#ifndef _ANYARCHIVE_H
#define _ANYARCHIVE_H

#include "AnyC64File.h"

/*! @class    AnyArchive
 *  @brief    This class adds an API to AnyC64File for handling file formats
 *            that store a collection of multiple C64 files (archives).
 */
class AnyArchive : public AnyC64File {
    
protected:
        
    /*! @brief    File pointer
     *  @details  An offset into the data range of the selected item.
     */
    long iFp = -1;
    
    /*! @brief    End of file position
     *  @details  This value equals the last valid offset plus 1
     */
    long iEof = -1;
    
public:

    //
    //! @functiongroup Creating and destructing objects
    //
    
    /*! @brief    Factory method
     *  @return   A T64File, D64File, PRGFile, P00File, or G64File object,
     *            depending on the type of the specified file.
     */
    static AnyArchive *makeWithFile(const char *filename);
    

    //
    //! @functiongroup Selecting an item
    //
    
    //! @brief    Returns the number of items in this archive.
    virtual int numberOfItems() { return 0; }
    
    /*! @brief    Selects the active item
     *  @details  All item related methods work on the active item.
     */
    virtual void selectItem(unsigned item) { };
    
    
    //
    //! @functiongroup Accessing item attributes
    //
    
    /*! @brief      Returns a string representation of the item type.
     *  @details    E.g., "PRG" for a program file.
     */
    virtual const char *getTypeOfItemAsString() { return ""; }
    
    //! @brief    Returns the name of the selected item.
    virtual const char *getNameOfItem() { return ""; }

    /*! @brief   Returns the name of the selected item in unichar format.
     *  @details The returned unichars are compatible with font C64ProMono
     *           which is used, e.g., in the mount dialogs preview panel.
     *  @note    Never call this function for nonexisting items.
     */
    const unsigned short *getUnicodeNameOfItem();
    

    //
    //! @functiongroup Reading data from the file
    //
    
    //! @brief    Returns the size of an item in bytes
    virtual size_t getSizeOfItem();
    
    //! @brief    Returns the size of an item in blocks
    virtual size_t getSizeOfItemInBlocks() { return (getSizeOfItem() + 253) / 254; }

    //! @brief    Moves the file pointer to the specified offset.
    /*! @details  Use seek(0) to return to the beginning of the selected item.
     */
    virtual void seekItem(long offset) { };
    
    /*! @brief    Reads a byte from the selected item.
     *  @return   EOF (-1) if all bytes have been read in.
     */
    virtual int readItem();
    
    /*! @brief    Reads multiple bytes in form of a hex dump string.
     *  @param    Number of bytes ranging from 1 to 85.
     */
    virtual const char *readItemHex(size_t num);
    
    /*! @brief    Returns the proposed memory location of the selected item.
     *  @details  When a file is flashed into memory, the item data is copied
     *            to this location.
     *  @seealso  flashItem()
     */
    virtual u16 getDestinationAddrOfItem() { return 0; }
    
    /*! @brief    Flashes the selected item into memory.
     *  @param    buffer must be a pointer to the C64 RAM
     */
    void flashItem(u8 *buffer);
    
 
    //
    //! @functiongroup Debugging
    //
    
    virtual void dumpDirectory(); 
    
};

#endif

