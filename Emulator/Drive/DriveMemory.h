// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _VC1541MEMORY_INC
#define _VC1541MEMORY_INC

#include "Memory.h"

class VC1541;

/*! @brief    Represents RAM and ROM of a virtual VC1541 floopy disk drive.
 */
class VC1541Memory : public Memory {
    
    private:
    
	//! @brief    Reference to the connected disk drive
	VC1541 *drive;
	
    public:
    
    //! @brief    Random Access Memory
    u8 ram[0x0800];
    
    //! @brief    Read Only Memory
    u8 rom[0x4000];
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
	//! @brief    Constructor
	VC1541Memory(VC1541 *drive);
	
	//! @brief    Destructor
	~VC1541Memory();

    
    //
    //! @functiongroup Methods from HardwareComponent
    //

	void reset();
	void dump();

    
    //
    //! @functiongroup Accessing ROM
    //

	//! @brief    Returns true iff the ROM image has been loaded.
    bool romIsLoaded() { return (rom[0] | rom[1]) != 0x00; }

    //! @brief    Removes the ROM image from memory
    void deleteRom() { memset(rom, 0, sizeof(rom)); }

    /*! @brief    Computes a 64-bit fingerprint for the VC1541 ROM.
     *  @return   fingerprint or 0, if no Basic ROM is installed.
     */
    u64 romFingerprint() {
        return romIsLoaded() ? fnv_1a_64(rom, sizeof(rom)) : 0; }
    
    
    //
    //! @functiongroup Accessing RAM
    //

    // Reading from memory
    u8 peek(u16 addr);
    u8 peekZP(u8 addr) { return ram[addr]; }

    // Reading from memory without side effects
    u8 spypeek(u16 addr);
    
    // Writing into memory
    void poke(u16 addr, u8 value);
    void pokeZP(u8 addr, u8 value) { ram[addr] = value; }
};

#endif
