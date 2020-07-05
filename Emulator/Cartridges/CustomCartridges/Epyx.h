// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _EPYX_H
#define _EPYX_H

#include "Cartridge.h"

class EpyxFastLoad : public Cartridge {
    
private:
    
    // Indicates when the capacitor discharges. The Epyx cartridge utilizes a
    // capacitor to switch the ROM on and off. During normal operation, the
    // capacitor charges slowly. When it is completely charged, the ROM gets
    // disabled. When the cartridge is attached, the capacitor is discharged
    // and the ROM visible. To avoid the ROM to be disabled, the cartridge can
    // either read from ROML or I/O space 1. Both operations discharge the
    // capacitor and keep the ROM alive.
    
    u64 cycle = 0;
    
public:
    
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() override { return CRT_EPYX_FASTLOAD; }
    
    
    //
    // Methods from HardwareComponent
    //
    
private:

    void reset() override;
    size_t stateSize() override;
    void didLoadFromBuffer(u8 **buffer) override;
    void didSaveToBuffer(u8 **buffer) override;
    
    
    //
    // Methods from Cartridge
    //
    
public:
    
    void resetCartConfig() override;
    u8 peekRomL(u16 addr) override;
    u8 spypeekRomL(u16 addr) { return Cartridge::peekRomL(addr); }
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) override { return 0; }
    u8 peekIO2(u16 addr) override;
    void execute() override;

private:
    
    // Discharges the cartridge's capacitor
    void dischargeCapacitor();
};

#endif
