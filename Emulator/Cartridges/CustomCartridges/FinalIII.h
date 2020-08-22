// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FINALIII_H
#define _FINALIII_H

#include "Cartridge.h"

class FinalIII : public Cartridge {
    
    // Indicates whether the freeze button is pressed
    bool freeezeButtonIsPressed = false;
    
    /* The QD pin of the Final Cartridge III's 4-bit counter. The counter's
     * purpose is to delay the grounding the Game line when the freeze button
     * is pressed. Doing so lets the CPU read the NMI vector with the old
     * Game/Exrom combination.
     */
    bool qD = true;

    
    //
    // Initializing
    //

public:
    
    FinalIII(C64 *c64, C64 &ref) : Cartridge(c64, ref, "FinalIII") { };
    CartridgeType getCartridgeType() override { return CRT_FINAL_III; }
    
    void resetCartConfig() override;

private:
    
    void _reset() override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker & freeezeButtonIsPressed;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker & qD;
    }
    
    size_t __size() { COMPUTE_SNAPSHOT_SIZE }
    size_t __load(u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    size_t __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    size_t _size() override { return Cartridge::_size() + __size(); }
    size_t _load(u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    size_t _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }
    
    
    
    size_t oldStateSize() override {
        return Cartridge::oldStateSize() + 2;
    }
    void oldDidLoadFromBuffer(u8 **buffer) override
    {
        Cartridge::oldDidLoadFromBuffer(buffer);
        freeezeButtonIsPressed = (bool)read8(buffer);
        qD = (bool)read8(buffer);
    }
    void oldDidSaveToBuffer(u8 **buffer) override
    {
        Cartridge::oldDidSaveToBuffer(buffer);
        write8(buffer, (u8)freeezeButtonIsPressed);
        write8(buffer, (u8)qD);
    }
    
    //
    // Accessing cartridge memory
    //
    
public:
        
    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
    void pokeIO2(u16 addr, u8 value) override;
    void nmiDidTrigger() override;
    
    unsigned numButtons() override { return 2; }
    const char *getButtonTitle(unsigned nr) override;
    void pressButton(unsigned nr) override;
    void releaseButton(unsigned nr) override;
 
    //
    // Accessing the control register
    //
    
private:
    
    void setControlReg(u8 value);
    bool hidden() { return (control & 0x80) != 0; }
    bool nmi() { return (control & 0x40) != 0; }
    bool game() { return (control & 0x20) != 0; }
    bool exrom() { return (control & 0x10) != 0; }
    u8 bank() { return (control & 0x03); }
    
    /* Indicates if the control register is write enabled. Final Cartridge III
     * enables and disables the control register by masking the clock signal.
     */
    bool writeEnabled();

    /* Updates the NMI line. The NMI line is driven by the control register and
     * the current position of the freeze button.
     */
    void updateNMI();
    
    /* Updates the Game line. The game line is driven by the control register
     * and counter output qD.
     */
    void updateGame();
};

#endif

