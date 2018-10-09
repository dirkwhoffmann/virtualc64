/*!
 * @header      ExpansionPort.h
 * @author      Written by Dirk Hoffmann based on the original code by A. Carl Douglas.
 * @copyright   All rights reserved.
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

class ExpansionPort : public VirtualComponent {
 
private:
    
    /*! @brief    Attached cartridge
     *  @details  NULL, if no cartridge is plugged in.
     */
    Cartridge *cartridge;
    
    /*! @brief    Current value of game line
     *  @details  Equals 1, if no cartridge if attached.
     */
    bool gameLine;
    
    /*! @brief    Current value of exrom line
     *  @details  Equals 1, if no cartridge if attached.
     */
    bool exromLine;
    
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
    void loadFromBuffer(uint8_t **buffer);
    
    //! @brief    Method from VirtualComponent
    void saveToBuffer(uint8_t **buffer);
    
    //! @brief    Method from VirtualComponent
    void dumpState();	
    
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
    
    //! @brief    Returns the state of the game line
    bool getGameLine() { return gameLine; }
    
    /*! @brief    Sets the state of the game line
     *  @details  Value has an effect on the C64's peek sources and poke targets
     */
    void setGameLine(bool value);

    //! @brief    Returns the state of the exrom line
    bool getExromLine() { return exromLine; }

    /*! @brief    Sets the state of the exrom line
     *  @details  Value has an effect on the C64's peek sources and poke targets
     */
    void setExromLine(bool value);
    
    //! @brief    Returns true if a cartridge is attached to the expansion port
    bool getCartridgeAttached() { return cartridge != NULL; }

    //! @brief    Attaches an existing cartridge to the expansion port
    bool attachCartridge(Cartridge *c);

    //! @brief    Creates and attaches a GeoRAM cartridge
    bool attachGeoRamCartridge(uint32_t capacity);
    
    //! @brief    Removes a cartridge from the expansion port (if any)
    void detachCartridge();

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
    
    //! @brief    Returns true if the attached cartridge has a RAM backing battery.
    bool hasBattery() { return cartridge != NULL && cartridge->persistentRam; }

    //! @brief    Enables or disables RAM backing during a reset.
    void setBattery(bool value) { if (cartridge) cartridge->persistentRam = value; }
};
    
#endif
