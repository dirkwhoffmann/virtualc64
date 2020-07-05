// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PRGFILE_H
#define _PRGFILE_H

#include "AnyArchive.h"

/*! @class   PRGFile
 *  @brief   The PRGFile class declares the programmatic interface for a file
 *           in PRG format.
 */
class PRGFile : public AnyArchive {

public:

    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true if buffer contains a PRG file
    /*! @details  PRG files can only be determined by their suffix, so this
     *            function will return true unless you provide a buffer with
     *            less than two bytes.
     */
    static bool isPRGBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Returns true iff the specified file is a PRG file.
    static bool isPRGFile(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Standard constructor
    PRGFile();
    
    //! @brief    Factory method
    static PRGFile *makeWithBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Factory method
    static PRGFile *makeWithFile(const char *path);
    
    /*! @brief    Factory method
     *  @details  otherArchive can be of any archive type
     */
    static PRGFile *makeWithAnyArchive(AnyArchive *otherArchive);
    
    
    //
    //! @functiongroup Methods from AnyFile
    //

    C64FileType type() { return PRG_FILE; }
    const char *typeAsString() { return "PRG"; }
    bool hasSameType(const char *filename) { return isPRGFile(filename); }
    
    
    //
    //! @functiongroup Methods from AnyArchive
    //
    
    int numberOfItems() { return 1; }
    void selectItem(unsigned item);
    const char *getTypeOfItemAsString() { return "PRG"; }
    const char *getNameOfItem() { return "FILE"; }
    size_t getSizeOfItem() { return size - 2; }
    void seekItem(long offset);
    u16 getDestinationAddrOfItem();
};
#endif
