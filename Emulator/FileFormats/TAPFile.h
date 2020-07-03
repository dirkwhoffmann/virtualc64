// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Constructor
    TAPFile();
    
    //! @brief    Factory method
    static TAPFile *makeWithBuffer(const uint8_t *buffer, size_t length);
    
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
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    
    
    //
    //! @functiongroup Retrieving tape information
    //
    
    uint8_t TAPversion() { return data[0x000C]; }
    uint8_t *getData() { return &data[0x0014]; }
    size_t getSize() { return size - 0x14; }
};

#endif
