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

#ifndef _ROMFILE_INC
#define _ROMFILE_INC

#include "File.h"

/*! @class  ROMFile
 *  @brief  Represents a ROM image file.
 */
class ROMFile : public File {
    
private:
    
    //! @brief    Header signatures
    static const uint8_t magicBasicRomBytes[];
    static const uint8_t magicCharRomBytes[];
    static const uint8_t magicKernalRomBytes[];
    static const uint8_t magicVC1541RomBytes1[];
    static const uint8_t magicVC1541RomBytes2[];

    //! @brief    ROM type (Basic ROM, Kernal ROM, etc.)
    ContainerType romtype;

    //! @brief    ROM size in bytes
    uint16_t size;
    
    //! @brief    ROM data
    uint8_t data[0x4000];
    
public:
    
    //! @brief    Standard constructor
    ROMFile();
    
    //! @brief    Factory method
    static ROMFile *makeRomFileWithBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Factory method
    static ROMFile *makeRomFileWithFile(const char *filename);
    
    //! @brief    Standard destructor
    // ~ROMFile();
    
    //! @brief    Returns true iff buffer contains a ROM image
    static bool isRom(const uint8_t *buffer, size_t length);

    //! @brief    Returns true iff buffer contains a Basic ROM image
    static bool isBasicRom(const uint8_t *buffer, size_t length);

    //! @brief    Returns true iff buffer contains a Character ROM image
    static bool isCharRom(const uint8_t *buffer, size_t length);

    //! @brief    Returns true iff buffer contains a Kernal ROM image
    static bool isKernalRom(const uint8_t *buffer, size_t length);

    //! @brief    Returns true iff buffer contains a VC1541 ROM image
    static bool isVC1541Rom(const uint8_t *buffer, size_t length);
    
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
    // Virtual functions from File class
    //
    
    ContainerType type() { return romtype; }
    const char *typeAsString() { return "ROM"; }
    bool hasSameType(const char *filename);
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    
    
    //
    //! @functiongroup Handling ROM data
    //
    
    // uint8_t *getData() { return data; }
    // uint16_t getSize() { return size; }
    
    //! @brief   Flushes the ROM into the emulator
    //! @details Copies the ROM data to the specified location.
    void flash(uint8_t *buffer);
};
#endif
