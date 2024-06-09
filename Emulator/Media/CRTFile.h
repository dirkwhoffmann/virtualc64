// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "CartridgeTypes.h"
#include "AnyFile.h"

namespace vc64 {

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
    
    static string cartridgeTypeName(CartridgeType type);
    static bool isCompatible(const fs::path &path);
    static bool isCompatible(std::istream &stream);

    
    //
    // Initializing
    //
    
    CRTFile(const fs::path &path) throws { init(path); }
    CRTFile(const u8 *buf, isize len) throws { init(buf, len); }
    
    
    //
    // Methods from CoreObject
    //
    
    const char *objectName() const override { return "CRTFile"; }


    //
    // Methods from AnyFile
    //
    
    bool isCompatiblePath(const fs::path &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    FileType type() const override { return FILETYPE_CRT; }
    PETName<16> getName() const override;
    void finalizeRead() override;

    
    //
    // Analyzing the cartridge
    //
    
    // Returns the version number of the cartridge
    u16 cartridgeVersion() const;

    // Returns the size of the cartridge header
    u32 headerSize() const;

    // Returns the cartridge type (e.g., SimonsBasic, FinalIII)
    CartridgeType cartridgeType() const;

    // Returns a textual description for the cartridge type
    string cartridgeTypeName() const { return cartridgeTypeName(cartridgeType()); }

    // Checks whether the cartridge type is supported by the emulator, yet
    bool isSupported() const;
    
    // Returns the initial value of the Exrom line and the Game line
    bool initialExromLine() const { return data[0x18] != 0; }
    bool initialGameLine() const { return data[0x19] != 0; }
    
    
    //
    // Analyzing chip packages
    //
    
    // Returns how many chips are contained in this cartridge
    isize chipCount() const;

    // Returns where the data of a certain chip can be found
    u8 *chipData(isize nr) const;

    // Returns the size of the chip (8 KB or 16 KB)
    u16 chipSize(isize nr) const;

    // Returns the type of the chip (0 = ROM, 1 = RAM, 2 = Flash ROM)
    u16 chipType(isize nr) const;

    // Returns the bank number for this chip
    u16 chipBank(isize nr) const;

    // Returns the start of the chip rom in address space
    u16 chipAddr(isize nr) const;


    //
    // Debugging and repairing
    //

    // Prints some information about this cartridge
    void dump() const;
    
    // Fixes known inconsistencies of common CRT files
    void repair();
};

}
