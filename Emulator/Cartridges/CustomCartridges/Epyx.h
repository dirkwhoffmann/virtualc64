// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

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
    
    i64 cycle = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    Epyx(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "Epyx"; }
    CartridgeType getCartridgeType() const override { return CRT_EPYX_FASTLOAD; }
    
private:
    
    void _reset(bool hard) override;

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        if (hard) {
            
            worker
            
            << cycle;
        }
    }
    
    isize __size() { COMPUTE_SNAPSHOT_SIZE }
    isize __load(const u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    isize __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    isize _size() override { return Cartridge::_size() + __size(); }
    isize _load(const u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    isize _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }

 
    //
    // Methods from Cartridge
    //
    
public:
    
    void resetCartConfig() override;
    u8 peekRomL(u16 addr) override;
    u8 spypeekRomL(u16 addr) const override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override ;
    void execute() override;

private:
    
    // Discharges the cartridge's capacitor
    void dischargeCapacitor();
};
