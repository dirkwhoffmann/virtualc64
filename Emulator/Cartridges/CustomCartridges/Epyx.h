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

class Epyx : public Cartridge {
    
private:
    
    // Indicates when the capacitor discharges. The Epyx cartridge utilizes a
    // capacitor to switch the ROM on and off. During normal operation, the
    // capacitor charges slowly. When it is completely charged, the ROM gets
    // disabled. When the cartridge is attached, the capacitor is discharged
    // and the ROM visible. To avoid the ROM to be disabled, the cartridge can
    // either read from ROML or I/O space 1. Both operations discharge the
    // capacitor and keep the ROM alive.
    
    u64 cycle = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    Epyx(C64 &ref) : Cartridge(ref, "Epyx") { };
    CartridgeType getCartridgeType() override { return CRT_EPYX_FASTLOAD; }
    
private:
    
    void _reset() override;

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & cycle;
    }
    
    size_t __size() { COMPUTE_SNAPSHOT_SIZE }
    size_t __load(u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    size_t __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    size_t _size() override { return Cartridge::_size() + __size(); }
    size_t _load(u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    size_t _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }

    
    //
    // Methods from HardwareComponent
    //
    
private:

    size_t oldStateSize() override;
    void oldDidLoadFromBuffer(u8 **buffer) override;
    void oldDidSaveToBuffer(u8 **buffer) override;
    
    
    //
    // Methods from Cartridge
    //
    
public:
    
    void resetCartConfig() override;
    u8 peekRomL(u16 addr) override;
    u8 spypeekRomL(u16 addr) override { return Cartridge::peekRomL(addr); }
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) override { return 0; }
    u8 peekIO2(u16 addr) override;
    void execute() override;

private:
    
    // Discharges the cartridge's capacitor
    void dischargeCapacitor();
};

#endif
