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
        
    //! @brief    Resets the expansion port
    void reset();
    
    //! @brief    Dumps the current configuration into the message queue
    void ping();

    //! @brief    Returns the size of the internal state
    uint32_t stateSize();

    //! @brief    Loads the current state from a buffer
    void loadFromBuffer(uint8_t **buffer);
    
    //! @brief    Save the current state into a buffer
    void saveToBuffer(uint8_t **buffer);
    
    //! @brief    Prints debugging information
    void dumpState();	
    
    //! @brief    Returns true if cartride ROM is blended in at the specified location
    bool romIsBlendedIn(uint16_t addr);
    
    //! @brief    Peek fallthrough
    uint8_t peek(uint16_t addr);

    //! @brief    Peek fallthrough for I/O region
    uint8_t peekIO(uint16_t addr);

    //! @brief    Poke fallthrough
    void poke(uint16_t addr, uint8_t value);
    
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

    //! @brief    Informs the expansion port of a game line or exrom line change
    // void gameOrExromLineHasChanged();
    
    //! @brief    Returns true if a cartridge is attached to the expansion port
    bool getCartridgeAttached() { return cartridge != NULL; }

    //! @brief    Attaches a cartridge to the expansion port
    bool attachCartridge(Cartridge *c);

    //! @brief    Attaches a cartridge to the expansion port
    /*! @details  Cartridge is provided by a data buffer
     */
    bool attachCartridge(uint8_t **buffer, CartridgeType type);

    //! @brief    Attaches a cartridge to the expansion port
    /*! @details  Cartridge is provided by a CRTContainer
     */
    bool attachCartridge(CRTContainer *c);

    //! @brief    Removes a cartridge from the expansion port (if any)
    void detachCartridge();

    //! @brief    Press first button on cartridge
    void pressFirstButton() { if (cartridge) cartridge->pressFirstButton(); }

    //! @brief    Press second button on cartridge
    void pressSecondButton() { if (cartridge) cartridge->pressSecondButton(); }

};
    
#endif
