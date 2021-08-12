// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "CartridgeTypes.h"
#include "AnyFile.h"

/* For details about the .CRT format,
 * see: http://vice-emu.sourceforge.net/vice_16.html
 *      http://ist.uwaterloo.ca/~schepers/formats/CRT.TXT
 *
 *
 * As well read the Commodore 64 Programmers Reference Guide pages 260-267.
 */

class CRTFile : public AnyFile {
        
    // Maximum number of chip packets in a CRT file
    static const isize MAX_PACKETS = 128;

    // Number of chips contained in cartridge file
    isize numberOfChips = 0;
    
    // Indicates where each chip section starts
    u8 *chips[MAX_PACKETS] = {};

public:
    
    //
    // Class methods
    //
    
    static bool isCompatiblePath(const string &name);
    static bool isCompatibleStream(std::istream &stream);
        
    
    //
    // Methods from C64Object
    //
    
    const char *getDescription() const override { return "CRTFile"; }

        
    //
    // Methods from AnyFile
    //
    
    FileType type() const override { return FILETYPE_CRT; }
    PETName<16> getName() const override;
    isize readFromStream(std::istream &stream) override;

    
    //
    // Analyzing the cartridge
    //
    
    // Returns the version number of the cartridge
    u16 cartridgeVersion() const { return LO_HI(data[0x15], data[0x14]); }
    
    // Returns the size of the cartridge header
    u32 headerSize() const { return HI_HI_LO_LO(data[0x10], data[0x11], data[0x12], data[0x13]); }
    
    // Returns the cartridge type (e.g., SimonsBasic, FinalIII)
    CartridgeType cartridgeType() const;

    // Checks whether the cartridge type is supported by the emulator, yet
    bool isSupported() const;
    
    // Returns the initial value of the Exrom line and the Game line
    bool initialExromLine() const { return data[0x18] != 0; }
    bool initialGameLine() const { return data[0x19] != 0; }
    
    
    //
    // Analyzing chip packages
    //
    
    // Returns how many chips are contained in this cartridge
    u8 chipCount() const { return numberOfChips; }
    
    // Returns where the data of a certain chip can be found
    u8 *chipData(isize nr) const { return chips[nr]+0x10; }
    
    // Returns the size of the chip (8 KB or 16 KB)
    u16 chipSize(isize nr) const { return LO_HI(chips[nr][0xF], chips[nr][0xE]); }
    
    // Returns the type of the chip (0 = ROM, 1 = RAM, 2 = Flash ROM)
    u16 chipType(isize nr) const { return LO_HI(chips[nr][0x9], chips[nr][0x8]); }
    
    // Returns the bank number for this chip
    u16 chipBank(isize nr) const { return LO_HI(chips[nr][0xB], chips[nr][0xA]); }
        
    // Returns the start of the chip rom in address space
    u16 chipAddr(isize nr) const { return LO_HI(chips[nr][0xD], chips[nr][0xC]); }


    //
    // Debugging, scanning and repairing a CRT file
    //

    // Prints some information about this cartridge
    void dump() const;
    
    void repair() override;
};
