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

#ifndef _G64FILE_INC
#define _G64FILE_INC

#include "AnyDisk.h"
#include "Disk.h"

/*! @class   G64File
 *  @brief   The G64File class declares the programmatic interface for a file
 *           in G64 format.
 */
class G64File : public AnyDisk {

private:

    //! @brief    Header signature
    static const uint8_t magicBytes[];
    
    /*! @brief    Number of the currently selected halftrack
     *  @details  0, if no halftrack is selected
     */
    Halftrack selectedHalftrack  = 0;
    
public:
    
    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a G64 file
    static bool isG64Buffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a G64 file
    static bool isG64File(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing G64 archives
    //
    
    //! @brief    Standard constructor
    G64File();

    //! @brief    Creates an empty G64 container with the specified capacity
    G64File(size_t capacity);

    //! @brief    Factory method
    static G64File *makeObjectWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static G64File *makeObjectWithFile(const char *path);
    
    //! @brief    Factory method
    static G64File *makeObjectWithDisk(Disk *disk);
    
    
    //
    // Methods from AnyC64File
    //
    
    C64FileType type() { return G64_FILE; }
    const char *typeAsString() { return "G64"; }
    bool hasSameType(const char *filename) { return G64File::isG64File(filename); }
    
    
    //
    // Methods from AnyArchive (API not supported by G64 format)
    //
    
    int numberOfItems() { assert(false); return 0; };
    size_t getSizeOfItem(unsigned n) { assert(false); return 0; }
    const char *getNameOfItem() { assert(false); return ""; }
    const char *getTypeOfItem() { assert(false); return ""; }
    uint16_t getDestinationAddrOfItem() { assert(false); return 0; }
    void selectItem(unsigned n) { assert(false); }
    uint32_t getStartOfItem(unsigned n) { assert(false); return 0; }

    
    //
    // Methods from AnyDisk
    //
    
    int numberOfHalftracks() { return 84; }
    void selectHalftrack(Halftrack ht);
    size_t getSizeOfHalftrack();
    void seekHalftrack(long offset);
    
private:
    
    long getStartOfHalftrack(Halftrack ht);
};

#endif

