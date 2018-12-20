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
    Cartridge *cartridge = NULL;
    
    /*! @brief    Current value of the Game line in phase phi1 (VICII access)
     *  @details  Equals 1, if no cartridge if attached.
     */
    bool gameLinePhi1 = 1;

    /*! @brief    Current value of the Game line in phase phi2 (CPU access)
     *  @details  Equals 1, if no cartridge if attached.
     */
    bool gameLinePhi2 = 1;
    
    /*! @brief    Current value of the Exrom line in phase phi1 (VICII access)
     *  @details  Equals 1, if no cartridge if attached.
     */
    bool exromLinePhi1 = 1;

    /*! @brief    Current value of the Exrom line in phase phi2 (CPU access)
     *  @details  Equals 1, if no cartridge if attached.
     */
    bool exromLinePhi2 = 1;
    
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
    bool getGameLinePhi1() { return gameLinePhi1; }
    bool getGameLinePhi2() { return gameLinePhi2; }

    /*! @brief    Sets the state of the Game line
     *  @details  Value has an effect on the C64's peek sources and poke targets
     */
    void setGameLinePhi1(bool value);
    void setGameLinePhi2(bool value);
    void setGameLine(bool value) { setGameLinePhi1(value); setGameLinePhi2(value); }
    
    //! @brief    Returns the state of the Exrom line
    bool getExromLinePhi1() { return exromLinePhi1; }
    bool getExromLinePhi2() { return exromLinePhi2; }

    /*! @brief    Sets the state of the Exrom line
     *  @details  Value has an effect on the C64's peek sources and poke targets
     */
    void setExromLinePhi1(bool value);
    void setExromLinePhi2(bool value);
    void setExromLine(bool value) { setExromLinePhi1(value); setExromLinePhi2(value); }
    
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
    uint8_t getSwitch() { return cartridge ? cartridge->getSwitch() : 0; }

    //! @brief    Puts the switch in the provided position
    void setSwitch(uint8_t pos) { if (cartridge) cartridge->setSwitch(pos); }

    //! @brief    Returns true if the attached cartridge has a RAM backing battery.
    bool hasBattery() { return cartridge != NULL && cartridge->persistentRam; }

    //! @brief    Enables or disables RAM backing during a reset.
    void setBattery(bool value) { if (cartridge) cartridge->persistentRam = value; }
};
    
#endif
