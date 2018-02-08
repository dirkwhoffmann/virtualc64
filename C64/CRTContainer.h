/*!
 * @header      CRTContainer.h
 * @author      Written by Dirk Hoffmann based on the original code by A. Carl Douglas.
 * @copyright   All rights reserved.
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

#ifndef _CRTContainer_H
#define _CRTContainer_H

#include "Container.h"

/*! @class    CRTContainer
 *  @brief    The CRTContainer class declares the programmatic interface for a file
 *            of the CRT format type.
 */
class CRTContainer : public Container {
    
private:
    
    //! @brief    Header signature
    static const uint8_t magicBytes[];
    
    //! @brief    Raw data of CRT container file
    uint8_t *data;
    
    //! @brief    Number of chips contained in cartridge file
    unsigned int numberOfChips;
    
    //! @brief    Indicates where each chip section starts
    uint8_t *chips[64];

public:
    
    //
    //! @functiongroup Creating and destructing containers
    //

    //! @brief    Constructor
    CRTContainer();
    
    //! @brief    Factory method
    static CRTContainer *makeCRTContainerWithBuffer(const uint8_t *buffer, size_t length);

    //! @brief    Factory method
    static CRTContainer *makeCRTContainerWithFile(const char *filename);

    //! @brief    Destructor
    ~CRTContainer();
    
    //! @brief    Frees the allocated memory.
    void dealloc();
    
    //
    //! @functiongroup Accessing container attributes
    //

    //! @brief    Returns the container type as numerical index
    ContainerType type() { return CRT_CONTAINER; }
    
    //! @brief    Returns the container type in plain text
    const char *typeAsString() { return "CRT"; }
    
    //! Return logical cartridge name
    //! TODO: Override getName
    char *cartridgeName() { return (char *)&data[0x20]; }
    
    //! @brief    Returns the version number of the cartridge
    uint16_t cartridgeVersion() { return LO_HI(data[0x15], data[0x14]); }
    
    //! @brief    Returns the cartridge type (e.g., SimonsBasic, FinalIII)
    /*! @details  Don't confuse with ContainerType
     */
    CartridgeType cartridgeType() { return CartridgeType(LO_HI(data[0x17], data[0x16])); }
    
    //! @brief    Returns the cartridge type in plain text
    const char *cartridgeTypeName();
    
    //! @brief    Returns the initial value of the Exrom line
    bool exromLine() { return data[0x18] != 0; }
    
    //! @brief    Returns the initial value of the Game line
    bool gameLine() { return data[0x19] != 0; }
    
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
    
    
    //
    //! @functiongroup Serializing
    //

    //! @brief    Returns true iff buffer contains a CRT file
    static bool isValidCRTBuffer(const uint8_t *buffer, size_t length);
    
    //! Returns true of filename points to a valid file of that type
    static bool isValidCRTFile(const char *filename);

    //! Check file type
    bool hasSameType(const char *filename) { return CRTContainer::isValidCRTFile(filename); }

    //! Read container data from memory buffer
    bool readFromBuffer(const uint8_t *buffer, size_t length);

};

#endif
