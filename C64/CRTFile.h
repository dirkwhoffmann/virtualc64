/*!
 * @header      CRTFile.h
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

/* For details about the .CRT format,
 * see: http://vice-emu.sourceforge.net/vice_16.html
 *      http://ist.uwaterloo.ca/~schepers/formats/CRT.TXT
 *
 *
 * As well read the Commodore 64 Programmers Reference Guide pages 260-267.
 */

#ifndef _CRTFILE_INC
#define _CRTFILE_INC

#include "AnyC64File.h"

/*! @class    CRTFile
 *  @brief    Represents a file of the CRT format type (cartridges).
 */
class CRTFile : public AnyC64File {
    
private:
    
    //! @brief    Header signature
    static const uint8_t magicBytes[];
        
    //! @brief    Number of chips contained in cartridge file
    unsigned int numberOfChips = 0;
    
    //! @brief    Indicates where each chip section starts
    uint8_t *chips[64];

public:
    
    //
    //! @functiongroup Class methods
    //
    
    //! @brief    Returns true if buffer contains a CRT file.
    static bool isCRTBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns the cartridge type number stored in the CRT buffer.
    static CartridgeType typeOfCRTBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns the cartridge type name stored in the CRT buffer.
    static const char *typeNameOfCRTBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true if buffer contains a supported CRT file.
    static bool isSupportedCRTBuffer(const uint8_t *buffer, size_t length);
    
    //! @brief    Returns true if buffer contains a CRT file of unsupported type.
    static bool isUnsupportedCRTBuffer(const uint8_t *buffer, size_t length);

    //! @brief    Returns true if path points to a CRT file.
    static bool isCRTFile(const char *path);
    
    
    //
    //! @functiongroup Creating and destructing objects
    //

    //! @brief    Constructor
    CRTFile();
    
    //! @brief    Factory method
    static CRTFile *makeObjectWithBuffer(const uint8_t *buffer, size_t length);

    //! @brief    Factory method
    static CRTFile *makeObjectWithFile(const char *filename);
    
    
    //
    //! @functiongroup Methods from AnyC64File
    //
    
    void dealloc();
    C64FileType type() { return CRT_FILE; }
    const char *typeAsString() { return "CRT"; }
    const char *getName() { return (char *)&data[0x20]; }
    bool hasSameType(const char *filename) { return CRTFile::isCRTFile(filename); }
    bool readFromBuffer(const uint8_t *buffer, size_t length);
    
    
    //
    //! @functiongroup Retrieving cartridge information
    //
    
    //! @brief    Returns the version number of the cartridge.
    uint16_t cartridgeVersion() { return LO_HI(data[0x15], data[0x14]); }
    
    /*! @brief    Returns the cartridge type (e.g., SimonsBasic, FinalIII)
     *  @details  Don't confuse with ContainerType
     */
    CartridgeType cartridgeType();

    //! @brief    Returns the cartridge type in plain text
    static const char *cartridgeTypeName(CartridgeType type);

    //! @brief    Returns the cartridge type in plain text
    const char *cartridgeTypeName();
    
    //! @brief    Returns the initial value of the Exrom line
    bool initialExromLine() { return data[0x18] != 0; }
    
    //! @brief    Returns the initial value of the Game line
    bool initialGameLine() { return data[0x19] != 0; }
    
    
    //
    //! @functiongroup Retrieving chip information
    //
    
    //! @brief    Returns how many chips are contained in this cartridge
    uint8_t chipCount() { return numberOfChips; }
    
    //! @brief    Returns where the data of a certain chip can be found
    uint8_t *chipData(unsigned nr) { return chips[nr]+0x10; }
    
    //! @brief    Returns the size of chip (8 KB or 16 KB)
    uint16_t chipSize(unsigned nr) { return LO_HI(chips[nr][0xF], chips[nr][0xE]); }
    
    //! @brief    Returns the type of chip
    /*! @details  0 = ROM, 1 = RAM, 2 = Flash ROM
     */
    uint16_t chipType(unsigned nr) { return LO_HI(chips[nr][0x9], chips[nr][0x8]); }
    
    //! @brief    Return bank information (what is this exactly?)
    uint16_t chipBank(unsigned nr) { return LO_HI(chips[nr][0xB], chips[nr][0xA]); }
    
    //! Returns start of chip rom in address space
    uint16_t chipAddr(unsigned nr) { return LO_HI(chips[nr][0xD], chips[nr][0xC]); }
    
};

#endif
