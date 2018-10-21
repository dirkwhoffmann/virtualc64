/*!
 * @header      TAPFile.h
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

#ifndef _TAPFILE_INC
#define _TAPFILE_INC

#include "AnyC64File.h"

/*! @class  TAPFile
 *  @brief  Represents a file of the TAP format type (band tapes).
 */
class TAPFile : public AnyC64File {
    
private:
    
    //! @brief    Header signature
    static const uint8_t magicBytes[];
    
    /*! @brief    File pointer
     *  @details  An offset into the data array. 
     */
    int fp;
    
public:
    
    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a TAP file
    static bool isTAPBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a TAP file
    static bool isTAPFile(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing objects
    //
    
    //! @brief    Constructor
    TAPFile();
    
    //! @brief    Factory method
    static TAPFile *makeObjectWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static TAPFile *makeObjectWithFile(const char *filename);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    void dealloc();
    C64FileType type() { return TAP_FILE; }
    const char *typeAsString() { return "TAP"; }
    const char *getName();
    bool hasSameType(const char *filename) { return isTAPFile(filename); }
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    size_t writeToBuffer(uint8_t *buffer);
    
    
    //
    //! @functiongroup Retrieving tape information
    //
    
    uint8_t TAPversion() { return data[0x000C]; }
    uint8_t *getData() { return &data[0x0014]; }
    size_t getSize() { return size - 0x14; }
};

#endif
