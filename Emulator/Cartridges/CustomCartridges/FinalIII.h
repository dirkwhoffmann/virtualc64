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

class FinalIII : public CartridgeWithRegister {
    
    //! @brief    Indicates if the freeze button is currenty pressed.
    bool freeezeButtonIsPressed;
    
    /*! @brief    The QD pin of the Final Cartridge III's 4-bit counter.
     *  @details  The counter's purpose is to delay grounding the Game line
     *            when the freeze button is pressed. Doing so lets the
     *            CPU read the NMI vector with the old Game/Exrom combination.
     */
    bool qD;

public:
    
    FinalIII(C64 *c64, C64 &ref) : CartridgeWithRegister(c64, ref, "FinalIII") { };
    CartridgeType getCartridgeType() { return CRT_FINAL_III; }
    
    //
    //! @functiongroup Methods from HardwareComponent
    //
    
    void reset();
    
    size_t stateSize() {
        return CartridgeWithRegister::stateSize() + 2;
    }
    void didLoadFromBuffer(u8 **buffer)
    {
        CartridgeWithRegister::didLoadFromBuffer(buffer);
        freeezeButtonIsPressed = (bool)read8(buffer);
        qD = (bool)read8(buffer);
    }
    void didSaveToBuffer(u8 **buffer)
    {
        CartridgeWithRegister::didSaveToBuffer(buffer);
        write8(buffer, (u8)freeezeButtonIsPressed);
        write8(buffer, (u8)qD);
    }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void resetCartConfig();
    
    u8 peekIO1(u16 addr);
    u8 peekIO2(u16 addr);
    void pokeIO2(u16 addr, u8 value);
    void nmiDidTrigger();
    
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    unsigned numButtons() { return 2; }
    const char *getButtonTitle(unsigned nr);
    void pressButton(unsigned nr);
    void releaseButton(unsigned nr);
 
    //! @brief    Writes a new value into the control register.
    void setControlReg(u8 value);

    bool hidden() { return (control & 0x80) != 0; }
    bool nmi() { return (control & 0x40) != 0; }
    bool game() { return (control & 0x20) != 0; }
    bool exrom() { return (control & 0x10) != 0; }
    u8 bank() { return (control & 0x03); }
    
    /*! @brief    Indicates if the control register is write enabled.
     *  @note     Final Cartridge III enables and disables the control register
     *            by masking the clock signal.
     */
    bool writeEnabled();

    /*! @brief    Updates the NMI line
     *  @note     The NMI line is driven by the control register and the
     *            current position of the freeze button.
     */
    void updateNMI();
    
    /*! @brief    Updates the Game line
     *  @note     The game line is driven by the control register and counter
     *            output qD.
     */
    void updateGame();
};

#endif

