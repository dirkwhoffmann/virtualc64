/*!
 * @header      G64File.h
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

#ifndef _G64ARCHIVE_INC
#define _G64ARCHIVE_INC

#include "AnyArchive.h"

// Forward declarations
class Disk;

/*! @class   G64File
 *  @brief   The G64File class declares the programmatic interface for a file
 *           in G64 format.
 */
class G64File : public AnyArchive {

public:

    //! @brief    Header signature
    static const uint8_t magicBytes[];
    
    /*! @brief    Number of the currently selected halftrack
     *  @details  -1, if no halftrack is selected
     */
    long selectedHalftrack  = -1;
    

    //
    //! @functiongroup Creating and destructing G64 archives
    //
    
    //! @brief    Standard constructor
    G64File();

    //! @brief    Creates an empty G64 container with the specified capacity
    G64File(size_t capacity);

    //! @brief    Factory method
    static G64File *makeG64ArchiveWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static G64File *makeG64ArchiveWithFile(const char *path);

    //! @brief    Factory method
    static G64File *makeG64ArchiveWithDisk(Disk *disk);
    
    //! @brief    Returns true iff buffer contains a G64 file
    static bool isG64(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a G64 file
    static bool isG64File(const char *filename);
    
    
    //
    // Methods from AnyFile
    //
    
    const char *getName();
    C64FileType type() { return G64_FILE; }
    const char *typeAsString() { return "G64"; }
    bool hasSameType(const char *filename) { return G64File::isG64File(filename); }
    size_t writeToBuffer(uint8_t *buffer);
    
    
    //
    // Methods from AnyArchive
    //
    
    int numberOfItems();
    size_t getSizeOfItem(unsigned n);
    const char *getNameOfItem();
    const char *getTypeOfItem(unsigned n);
    uint16_t getDestinationAddrOfItem(unsigned n) { return 0; }
    void selectItem(unsigned n);
    uint32_t getStartOfItem(unsigned n);
};


#endif

