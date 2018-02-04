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

#ifndef _P00ARCHIVE_INC
#define _P00ARCHIVE_INC

#include "Archive.h"

/*! @class  D64Archive
 *  @brief  The D64Archive class declares the programmatic interface for a file in P00 format.
 */
class P00Archive : public Archive {

private:

    //! @brief    Header signature
    static const uint8_t magicBytes[];
    
    //! @brief    The raw data of this archive.
    uint8_t *data;
		
    //! @brief    File size
	size_t size;

    /*! @brief    File pointer
     *  @details  An offset into the data array. 
     */
    int fp;

public:

    //! @brief    Standard constructor.
    P00Archive();
    
    //! @brief    Factory method
    static P00Archive *makeP00ArchiveWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static P00Archive *makeP00ArchiveWithFile(const char *path);
    
    /*! @brief    Factory method
     *  @details  otherArchive can be of any archive type
     */
    static P00Archive *makeP00ArchiveWithAnyArchive(Archive *otherArchive);
    
    //! @brief    Standard destructor.
    ~P00Archive();
    
    //! @brief    Returns true iff buffer contains a P00 file
    static bool isP00(const uint8_t *buffer, size_t length);

    //! @brief    Returns true iff the specified file is a P00 file
    static bool isP00File(const char *filename);
    
    //
    // Virtual functions from Container class
    //
    
    void dealloc();
    
    const char *getName();
    ContainerType type() { return P00_CONTAINER; }
    const char *typeAsString() { return "P00"; }
    
    bool hasSameType(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    size_t writeToBuffer(uint8_t *buffer);
    
    
    //
    // Virtual functions from Archive class
    //
    
    int getNumberOfItems();
    
    const char *getNameOfItem(int n);
    const unsigned short *getUnicodeNameOfItem(int n, size_t maxChars);
    const char *getTypeOfItem(int n);
    uint16_t getDestinationAddrOfItem(int n);
    
    void selectItem(int n);
    int getByte();
};
#endif
