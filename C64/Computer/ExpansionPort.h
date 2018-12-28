/*!
 * @header      ExpansionPort.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
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

/*
 * For more information: http://www.c64-wiki.com/index.php/Cartridge
 *
 * "The cartridge system implemented in the C64 provides an easy way to
 *  hook 8 or 16 kilobytes of ROM into the computer's address space:
 *  This allows for applications and games up to 16 K, or BASIC expansions
 *  up to 8 K in size and appearing to the CPU along with the built-in
 *  BASIC ROM. In theory, such a cartridge need only contain the
 *  ROM circuit without any extra support electronics."
 *
 *  Bank switching info: http://www.c64-wiki.com/index.php/Bankswitching
 *                       http://www.harries.dk/files/C64MemoryMaps.pdf
 *
 *  As well read the Commodore 64 Programmers Reference Guide pages 260-267.
 */

#ifndef _EXPANSIONPORT_H
#define _EXPANSIONPORT_H

#include "Cartridge.h"
#include "ExpansionPort_types.h"

class ExpansionPort : public VirtualComponent {
 
private:
    
    /*! @brief    Attached cartridge
     *  @details  NULL, if no cartridge is plugged in.
     */
    Cartridge *cartridge = NULL;
    
    /*! @brief    Current value of the Game line.
     *  @details  Equals 1, if no cartridge if attached.
     */
    bool gameLine = 1;
    
    /*! @brief    Current value of the Exrom line.
     *  @details  Equals 1, if no cartridge if attached.
     */
    bool exromLine = 1;
    
public:
    
    //! @brief    Constructor
    ExpansionPort();
    
    //! @brief    Destructor
    ~ExpansionPort();
        
    //! @brief    Method from VirtualComponent
    void reset();
    
    //! @brief    Method from VirtualComponent
    void ping();

    //! @brief    Method from VirtualComponent
    size_t stateSize();

    //! @brief    Method from VirtualComponent
    void didLoadFromBuffer(uint8_t **buffer);
    
    //! @brief    Method from VirtualComponent
    void didSaveToBuffer(uint8_t **buffer);
    
    //! @brief    Method from VirtualComponent
    void dump();	
    
    //! @brief    Execution thread callback
    /*! @details  This method is invoked after each rasterline.
     */
    void execute() { if (cartridge) cartridge->execute(); }
    
    //! @brief    Peek fallthrough
    uint8_t peek(uint16_t addr);
    
    //! @brief    Same as peek, but without side effects
    uint8_t spypeek(uint16_t addr);
    
    //! @brief    Peek fallthrough for I/O space 1
    uint8_t peekIO1(uint16_t addr);
    
    //! @brief    Same as peekIO1, but without side effects
    uint8_t spypeekIO1(uint16_t addr);
    
    //! @brief    Peek fallthrough for I/O space 2
    uint8_t peekIO2(uint16_t addr);

    //! @brief    Same as peekIO2, but without side effects
    uint8_t spypeekIO2(uint16_t addr);
    
    //! @brief    Poke fallthrough
    void poke(uint16_t addr, uint8_t value);
    
    //! @brief    Poke fallthrough for I/O space 1
    void pokeIO1(uint16_t addr, uint8_t value);
    
    //! @brief    Poke fallthrough for I/O space 2
    void pokeIO2(uint16_t addr, uint8_t value);
    
    //! @brief    Returns the cartridge type
    CartridgeType getCartridgeType();
    
    //! @brief    Returns the state of the Game line
    bool getGameLine() { return gameLine; }

    /*! @brief    Sets the state of the Game line
     *  @note     This value affects the C64's and VICII's mem source table.
     */
    void setGameLine(bool value);
    
    //! @brief    Returns the state of the Exrom line
    bool getExromLine() { return exromLine; }

    /*! @brief    Sets the state of the Exrom line
     *  @note     This value affects the C64's and VICII's mem source table.
     */
    void setExromLine(bool value);
    
    /*! @brief    Returns the current cartridge mode.
     *  @details  The cartridge mode is determined by the current values of the
     *            Game and Exrom line.
     */
    CartridgeMode getCartridgeMode();
    
    //! @brief    Sets the state of the Game and Exrom line
    void setGameAndExrom(bool game, bool exrom);

    //! @brief    Convenience wrapper for setGame(), setExrom()
    void setCartridgeMode(CartridgeMode mode);

    
    /*! @brief    Modifies the memory source lookup tables if required
     *  @details  This function is called in C64::updatePeekPokeLookupTables()
     *            to allow cartridges to manipulate the lookup tables after the
     *            default values have been set.
     *            Background: Some cartridges such as StarDos change the game
     *            and exrom line on-the-fly to achieve very special memory
     *            mappings.
     *            For most cartridges, this function does nothing.
     */
    void updatePeekPokeLookupTables();
    
    //! @brief    Returns true if a cartridge is attached to the expansion port
    bool getCartridgeAttached() { return cartridge != NULL; }

    //! @brief    Attaches a cartridge to the expansion port.
    void attachCartridge(Cartridge *c);

    //! @brief    Attaches a cartridge from a file and resets.
    bool attachCartridgeAndReset(CRTFile *c);
    
    //! @brief    Creates and attaches a GeoRAM cartridge
    bool attachGeoRamCartridge(uint32_t capacity);

    //! @brief    Creates and attaches an Isepic cartridge
    void attachIsepicCartridge();

    //! @brief    Removes a cartridge from the expansion port (if any)
    void detachCartridge();

    //! @brief    Removes a cartridge from the expansion port and resets
    void detachCartridgeAndReset();

    //
    //! @functiongroup Operating cartridge buttons
    //
    
    //! @brief    Returns the number of available cartridge buttons
    virtual unsigned numButtons() { return cartridge ? cartridge->numButtons() : 0; }
    
    //! @brief    Returns a textual description for a button.
    virtual const char *getButtonTitle(unsigned nr) {
        return cartridge ? cartridge->getButtonTitle(nr) : NULL; }
    
    //! @brief    Presses a button
    virtual void pressButton(unsigned nr) { if (cartridge) cartridge->pressButton(nr); }
    
    //! @brief    Releases a button
    virtual void releaseButton(unsigned nr) { if (cartridge) cartridge->releaseButton(nr); }
    
    
    
    
    //! @brief    Returns true if a cartridge with a freeze button is attached
    bool hasFreezeButton() { return cartridge ? cartridge->hasFreezeButton() : false; }
    
    //! @brief    Presses the first cartridge button
    void pressFreezeButton() { if (cartridge) cartridge->pressFreezeButton(); }

    //! @brief    Releases the first cartridge button
    void releaseFreezeButton() { if (cartridge) cartridge->releaseFreezeButton(); }

    //! @brief    Returns true if a cartridge with a reset button is attached
    bool hasResetButton() { return cartridge ? cartridge->hasResetButton() : false; }
    
    //! @brief    Presses the second cartridge button
    void pressResetButton() { if (cartridge) cartridge->pressResetButton(); }
    
    //! @brief    Releases the second cartridge button
    void releaseResetButton() { if (cartridge) cartridge->releaseResetButton(); }
    
    //! @brief    Returns true if a cartridge with a switch is attached
    bool hasSwitch() { return cartridge ? cartridge->hasSwitch() : false; }
    
    //! @brief    Returns the current position of the switch
    int8_t getSwitch() { return cartridge ? cartridge->getSwitch() : 0; }
    bool switchIsNeutral() { return cartridge ? cartridge->switchIsNeutral() : false; }
    bool switchIsLeft() { return cartridge ? cartridge->switchIsLeft() : false; }
    bool switchIsRight() { return cartridge ? cartridge->switchIsRight() : false; }
    const char *getSwitchDescription(int8_t pos) {
        return cartridge ? cartridge->getSwitchDescription(pos) : NULL; }
    
    //! @brief    Puts the switch in the provided position
    void setSwitch(uint8_t pos) { if (cartridge) cartridge->setSwitch(pos); }

    //! @brief    Push to next switch position
    void toggleSwitch() { if (cartridge) cartridge->toggleSwitch(); }

    //! @brief    Returns true if the cartridge has a LED.
    bool hasLED() { return cartridge ? cartridge->hasLED() : false; }
    
    //! @brief    Returns true if the LED is switched on.
    bool getLED() { return cartridge ? cartridge->getLED() : false; }
    
    //! @brief    Switches the LED on or off.
    void setLED(bool value) { if (cartridge) cartridge->setLED(value); }
    
    //! @brief    Returns true if the attached cartridge has a RAM backing battery.
    bool hasBattery() { return cartridge ? cartridge->getPersistentRam() : false; }

    //! @brief    Enables or disables RAM backing during a reset.
    void setBattery(bool value) { if (cartridge) cartridge->setPersistentRam(value); }
    
    
    //
    // Notifications
    //
    
    // @brief    Called when the C64 CPU triggers an NMI
    void nmiWillTrigger() { if (cartridge) cartridge->nmiWillTrigger(); }
};
    
#endif
