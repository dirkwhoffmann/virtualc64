/*!
 * @header      PRGFile.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/*
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

#ifndef _PRGFILE_INC
#define _PRGFILE_INC

#include "AnyArchive.h"

/*! @class   PRGFile
 *  @brief   The PRGFile class declares the programmatic interface for a file
 *           in PRG format.
 */
class PRGFile : public AnyArchive {

    
public:

    //
    //! @functiongroup Creating and destructing PRG archives
    //
    
    //! @brief    Standard constructor
    PRGFile();
    
    //! @brief    Factory method
    static PRGFile *makePRGArchiveWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static PRGFile *makePRGArchiveWithFile(const char *path);
    
    /*! @brief    Factory method
     *  @details  otherArchive can be of any archive type
     */
    static PRGFile *makePRGArchiveWithAnyArchive(AnyArchive *otherArchive);
    
    //! @brief    Returns true if buffer contains a PRG file
    /*! @details  PRG files ares mostly determined by their suffix, so this function will
     *            return true unless you provide a buffer with less than two bytes.
     */
    static bool isPRG(const uint8_t *buffer, size_t length);

    //! @brief    Returns true iff the specified file is a PRG file.
    static bool isPRGFile(const char *filename);

    
    //
    // Methods from AnyFile
    //

    C64FileType type() { return PRG_FILE; }
    const char *typeAsString() { return "PRG"; }
    bool hasSameType(const char *filename) { return isPRGFile(filename); }
    
    
    //
    // Methods from AnyArchive
    //
    
    int numberOfItems() { return 1; }
    // const char *getNameOfItem(unsigned n) { return "FILE"; }
    const char *getTypeOfItem(unsigned n) { return "PRG"; }
    uint16_t getDestinationAddrOfItem(unsigned n);
    void selectItem(unsigned n);
    
};
#endif
