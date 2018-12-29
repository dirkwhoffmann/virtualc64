/*!
 * @header      FinalIII.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
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

#ifndef _FINALIII_INC
#define _FINALIII_INC

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
    
    FinalIII(C64 *c64) : CartridgeWithRegister(c64, "FinalIII") { };
    CartridgeType getCartridgeType() { return CRT_FINAL_III; }
    
    //
    //! @functiongroup Methods from VirtualComponent
    //
    
    void reset();
    
    size_t stateSize() {
        return CartridgeWithRegister::stateSize() + 2;
    }
    void didLoadFromBuffer(uint8_t **buffer)
    {
        CartridgeWithRegister::didLoadFromBuffer(buffer);
        freeezeButtonIsPressed = (bool)read8(buffer);
        qD = (bool)read8(buffer);
    }
    void didSaveToBuffer(uint8_t **buffer)
    {
        CartridgeWithRegister::didSaveToBuffer(buffer);
        write8(buffer, (uint8_t)freeezeButtonIsPressed);
        write8(buffer, (uint8_t)qD);
    }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void resetCartConfig();
    
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO2(uint16_t addr, uint8_t value);
    void nmiDidTrigger();
    
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    unsigned numButtons() { return 2; }
    const char *getButtonTitle(unsigned nr);
    void pressButton(unsigned nr);
    void releaseButton(unsigned nr);
 
    //! @brief    Writes a new value into the control register.
    void setControlReg(uint8_t value);

    bool hidden() { return (control & 0x80) != 0; }
    bool nmi() { return (control & 0x40) != 0; }
    bool game() { return (control & 0x20) != 0; }
    bool exrom() { return (control & 0x10) != 0; }
    uint8_t bank() { return (control & 0x03); }
    
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

