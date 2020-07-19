// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _TAPFILE_H
#define _TAPFILE_H

#include "AnyFile.h"

/*! @class  TAPFile
 *  @brief  Represents a file of the TAP format type (band tapes).
 */
class TAPFile : public AnyFile {
    
private:
    
    //! @brief    Header signature
    static const u8 magicBytes[];
    
    /*! @brief    File pointer
     *  @details  An offset into the data array. 
     */
    int fp;
    
public:
    
    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a TAP file
    static bool isTAPBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a TAP file
    static bool isTAPFile(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Constructor
    TAPFile();
    
    //! @brief    Factory method
    static TAPFile *makeWithBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Factory method
    static TAPFile *makeWithFile(const char *filename);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    void dealloc();
    C64FileType type() { return TAP_FILE; }
    const char *typeAsString() { return "TAP"; }
    const char *getName();
    bool hasSameType(const char *filename) { return isTAPFile(filename); }
    bool readFromBuffer(const u8 *buffer, size_t length);
    
    
    //
    //! @functiongroup Retrieving tape information
    //
    
    u8 TAPversion() { return data[0x000C]; }
    u8 *getData() { return &data[0x0014]; }
    size_t getSize() { return size - 0x14; }
};

#endif
