/*!
 * @header      ROMFile.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
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

#ifndef _ROMFILE_INC
#define _ROMFILE_INC

#include "AnyC64File.h"

/*! @class  ROMFile
 *  @brief  Represents a ROM image file.
 */
class ROMFile : public AnyC64File {
    
private:

    //! @brief    Header signatures
    static const size_t basicRomSignatureCnt  = 1;
    static const size_t charRomSignatureCnt   = 10;
    static const size_t kernalRomSignatureCnt = 1;
    static const size_t vc1541RomSignatureCnt = 4;

    static const uint8_t magicBasicRomBytes[basicRomSignatureCnt][3];
    static const uint8_t magicCharRomBytes[charRomSignatureCnt][4];
    static const uint8_t magicKernalRomBytes[kernalRomSignatureCnt][3];
    static const uint8_t magicVC1541RomBytes[vc1541RomSignatureCnt][3];

    //! @brief    ROM type (Basic ROM, Kernal ROM, etc.)
    C64FileType romtype;
        
public:
    
    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a ROM image
    static bool isRomBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a Basic ROM image
    static bool isBasicRomBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a Character ROM image
    static bool isCharRomBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a Kernal ROM image
    static bool isKernalRomBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a VC1541 ROM image
    static bool isVC1541RomBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true iff filename points to a ROM file
    static bool isRomFile(const char *filename);
    
    //! @brief    Returns true iff filename points to a Basic ROM file
    static bool isBasicRomFile(const char *filename);
    
    //! @brief    Returns true iff filename points to a Character ROM file
    static bool isCharRomFile(const char *filename);
    
    //! @brief    Returns true iff filename points to a Kernal ROM file
    static bool isKernalRomFile(const char *filename);
    
    //! @brief    Returns true iff filename points to a VC1541 ROM file
    static bool isVC1541RomFile(const char *filename);
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Standard constructor
    ROMFile();
    
    //! @brief    Factory method
    static ROMFile *makeWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static ROMFile *makeWithFile(const char *filename);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    C64FileType type() { return romtype; }
    const char *typeAsString() { return "ROM"; }
    bool hasSameType(const char *filename) { return isRomFile(filename); }
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    
};
#endif
