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

#ifndef _TAPARCHIVE_INC
#define _TAPARCHIVE_INC

#include "Archive.h"

/*! @class  TAPArchive
 *  @brief  The TAPArchive class declares the programmatic interface for a file in TAP format.
 */
class TAPArchive : public Container {
    
private:
    
    //! @brief    The raw data of this archive.
    uint8_t *data;
    
    //! @brief    File size
    int size;
    
    /*! @brief    File pointer
     *  @details  An offset into the data array. 
     */
    int fp;
    
public:
    
    //! @brief    Standard constructor
    TAPArchive();
    
    //! @brief    Standard destructor
    ~TAPArchive();
    
    //! @brief    Returns true iff the specified file is a TAP file
    static bool isTAPFile(const char *filename);
    
    //! @brief    Creates a TAP archive from a TAP file.
    static TAPArchive *archiveFromTAPFile(const char *filename);
    
    
    //
    // Virtual functions from Container class
    //
    
    void dealloc();
    
    const char *getName();
    ContainerType getType() { return TAP_CONTAINER; }
    const char *getTypeAsString() { return "TAP"; }
    
    bool fileIsValid(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, unsigned length);
    unsigned writeToBuffer(uint8_t *buffer);
    
    
    //
    // Accessing the archive
    //
    
    unsigned TAPversion() { return data[0x000C]; }
    uint8_t *getData() { return &data[0x0014]; }
    int getSize() { return size - 0x14; }
};
#endif
