// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _ROMFILE_H
#define _ROMFILE_H

#include "AnyC64File.h"

/*! @class  ROMFile
 *  @brief  Represents a ROM image file.
 */
class ROMFile : public AnyC64File {
    
private:

    //! @brief    Header signatures
    static const size_t basicRomSignatureCnt  = 3;
    static const size_t charRomSignatureCnt   = 10;
    static const size_t kernalRomSignatureCnt = 3;
    static const size_t vc1541RomSignatureCnt = 4;

    static const u8 magicBasicRomBytes[basicRomSignatureCnt][3];
    static const u8 magicCharRomBytes[charRomSignatureCnt][4];
    static const u8 magicKernalRomBytes[kernalRomSignatureCnt][3];
    static const u8 magicVC1541RomBytes[vc1541RomSignatureCnt][3];

    //! @brief    ROM type (Basic ROM, Kernal ROM, etc.)
    C64FileType romtype;
        
public:
    
    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true iff buffer contains a ROM image
    static bool isRomBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a Basic ROM image
    static bool isBasicRomBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a Character ROM image
    static bool isCharRomBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a Kernal ROM image
    static bool isKernalRomBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Returns true iff buffer contains a VC1541 ROM image
    static bool isVC1541RomBuffer(const u8 *buffer, size_t length);
    
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
    static ROMFile *makeWithBuffer(const u8 *buffer, size_t length);
    
    //! @brief    Factory method
    static ROMFile *makeWithFile(const char *filename);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    C64FileType type() { return romtype; }
    const char *typeAsString() { return "ROM"; }
    bool hasSameType(const char *filename) { return isRomFile(filename); }
    bool readFromBuffer(const u8 *buffer, size_t length);
    
};
#endif
