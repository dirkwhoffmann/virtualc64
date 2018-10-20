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

#ifndef _TAPCONTAINER_INC
#define _TAPCONTAINER_INC

#include "AnyC64File.h"

/*! @class  TAPFile
 *  @brief  Represents a file of the TAP format type (band tapes).
 */
class TAPFile : public AnyC64File {
    
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
    
    //! @brief    Constructor
    TAPFile();
    
    //! @brief    Factory method
    static TAPFile *makeTAPContainerWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static TAPFile *makeTAPContainerWithFile(const char *filename);
    
    //! @brief    Returns true iff buffer contains a TAP file
    static bool isTAP(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a TAP file
    static bool isTAPFile(const char *filename);
    
    //
    // Virtual functions from Container class
    //
    
    void dealloc();
    
    const char *getName();
    ContainerType type() { return TAP_CONTAINER; }
    const char *typeAsString() { return "TAP"; }
    
    bool hasSameType(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    size_t writeToBuffer(uint8_t *buffer);
    
    
    //
    // Accessing the container
    //
    
    uint8_t TAPversion() { return data[0x000C]; }
    uint8_t *getData() { return &data[0x0014]; }
    size_t getSize() { return size - 0x14; }
};

#endif
