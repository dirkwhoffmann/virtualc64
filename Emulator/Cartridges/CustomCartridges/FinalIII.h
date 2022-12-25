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
    
    FinalIII(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "FinalIII"; }
    CartridgeType getCartridgeType() const override { return CRT_FINAL_III; }
    
    void resetCartConfig() override;

private:
    
    void _reset(bool hard) override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker << freeezeButtonIsPressed;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        worker << qD;
    }
    
    isize __size() override { COMPUTE_SNAPSHOT_SIZE }
    isize __load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize __save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
        

    //
    // Accessing cartridge memory
    //
    
public:
        
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO2(u16 addr, u8 value) override;
    void nmiDidTrigger() override;
    
 
    //
    // Accessing the control register
    //
    
private:
    
    void setControlReg(u8 value);
    bool hidden() const { return (control & 0x80) != 0; }
    bool nmi() const { return (control & 0x40) != 0; }
    bool game() const { return (control & 0x20) != 0; }
    bool exrom() const { return (control & 0x10) != 0; }
    u8 bank() const { return (control & 0x03); }
    
    /* Indicates if the control register is write enabled. Final Cartridge III
     * enables and disables the control register by masking the clock signal.
     */
    bool writeEnabled() const;

    /* Updates the NMI line. The NMI line is driven by the control register and
     * the current position of the freeze button.
     */
    void updateNMI();
    
    /* Updates the Game line. The game line is driven by the control register
     * and counter output qD.
     */
    void updateGame();
    
    
    //
    // Operating buttons
    //
    
    isize numButtons() const override { return 2; }
    const string getButtonTitle(isize nr) const override;
    void pressButton(isize nr) override;
    void releaseButton(isize nr) override;
};
