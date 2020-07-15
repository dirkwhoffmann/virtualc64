// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _P00FILE_H
#define _P00FILE_H

#include "AnyArchive.h"

/*! @class   P00File
 *  @brief   The P00File class declares the programmatic interface for a file
 *           in P00 format.
 */
class P00File : public AnyArchive {

private:

    //! @brief    Header signature
    static const u8 magicBytes[];
    
public:

    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a P00 file.
    static bool isP00Buffer(const u8 *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a P00 file.
    static bool isP00File(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Standard constructor
    P00File();
    
    //! @brief    Factory method
    static P00File *makeWithBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Factory method
    static P00File *makeWithFile(const char *path);
    
    /*! @brief    Factory method
     *  @details  otherArchive can be of any archive type
     */
    static P00File *makeWithAnyArchive(AnyArchive *otherArchive);
    
    
    //
    //! @functiongroup Methods from AnyFile
    //
    
    const char *getName();
    C64FileType type() { return P00_FILE; }
    const char *typeAsString() { return "P00"; }
    bool hasSameType(const char *filename) { return isP00File(filename); }
    
    
    //
    //! @functiongroup Methods from AnyArchive
    //
    
    int numberOfItems() { return 1; }
    void selectItem(unsigned item);
    const char *getTypeOfItemAsString() { return "PRG"; }
    const char *getNameOfItem();
    size_t getSizeOfItem() { return size - 0x1C; }
    void seekItem(long offset);
    u16 getDestinationAddrOfItem();
};
#endif
