/*!
 * @header      Cartridge.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
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

// TODO:
// 1. Implement peekLO, peekHI
// 2. Implement SimonsBasic class
// 3. Warn user if an unsupported cartridge type is plugged in
// 4. Implement Ocean ... class (Terminator 2)
// 5. Implement FinalCartridge class
// 6. Add MenuItem Cartridge->FinalCartridge

#ifndef _CARTRIDGE_INC
#define _CARTRIDGE_INC

#include "VirtualComponent.h"
#include "CRTContainer.h"

class ExpansionPort;

/*!
 * @brief    Cartridge that can be plugged into the C64's expansion port
 */
class Cartridge : public VirtualComponent {
    
private:
    
    /*! @brief    Type of the attached cartridge
     */
    CartridgeType type;
    
    /*! @brief    Game line of the attached cartridge
     */
    bool gameLine;
    
    /*! @brief    Exrom line of the attached cartridge
     */
    bool exromLine;
    
    /*! @brief    ROM chips contained in the attached cartridge
     *  @details  A cartridge can contain up to 64 chips
     */
    uint8_t *chip[64];
    
    //! @brief    Array containing the load addresses of all chips
    uint16_t chipStartAddress[64];
    
    //! @brief    Array containing the chip sizes of all chips
    uint16_t chipSize[64];
    
    //! @brief    Virtual cartridge ROM (32 kb starting at $8000)
    uint8_t rom[0x8000];
    
    /*! @brief    Indicates whether ROM is blended in (0x01) or or out (0x00)
     *  @details  Each array item represents a 4k block above $8000
     */
    uint8_t blendedIn[16];
    
    /*! @brief    Registered expansion port listener
     *! @details  If an expansion port is set, it is informed when gameLine or
     *            exromLine change their value.
     */
    ExpansionPort *listener;
    
public:
    
    //! @brief    Convenience constructor
    Cartridge(C64 *c64);

    //! @brief    Destructor
    ~Cartridge();
    
    //! @brief    Check cartridge type
    /*! @details  Returns true iff the cartridge type is supported.
     */
    static bool isSupportedType(CRTContainer *container);
    
    //! @brief    Factory method
    /*! @details  Creates a cartridge from a CRT container.
     *            Make sure that you only pass containers of supported cartridge type.
     *  @seealso  isSupportedType
     */
    static Cartridge *makeCartridgeWithCRTContainer(C64 *c64, CRTContainer *container);
    
    //! @brief    Factory method
    /*! @details  Creates a cartridge from a serialized data stream */
    static Cartridge *makeCartridgeWithBuffer(C64 *c64, uint8_t **buffer, CartridgeType type);
    
    //! @brief    Resets the cartridge
    void reset();
    
    //! @brief    Reverts cartridge to its initial state
    /*! @details  Switches back to first bank
     */
    void softreset();
    
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
    bool romIsBlendedIn(uint16_t addr) { return blendedIn[addr >> 12]; }
    
    //! @brief    Peek fallthrough
    uint8_t peek(uint16_t addr) { return rom[addr & 0x7FFF]; }
    
    //! @brief    Poke fallthrough
    void poke(uint16_t addr, uint8_t value);
    
    //! @brief    Returns the cartridge type
    CartridgeType getCartridgeType() { return type; }
    
    /*! @brief    Counts the number of chips
     *  @return   Value between 0 and 64
     */
    unsigned numberOfChips();
    
    //! @brief    Sums up the sizes of all chips in bytes
    unsigned numberOfBytes();
    
    //! @brief    Returns the state of the game line
    bool getGameLine() { return gameLine; }
    
    //! @brief    Sets the state of the game line
    void setGameLine(bool value);
    
    //! @brief    Returns the state of the exrom line
    bool getExromLine() { return exromLine; }
    
    //! @brief    Sets the state of the exrom line
    void setExromLine(bool value);
    
    //! @brief    Blends in a cartridge chip into the ROM address space
    void switchBank(unsigned nr);
    
    //! @brief    Attaches a single cartridge chip
    void attachChip(unsigned nr, CRTContainer *c);
    
    //! @brief    Sets the expansion port listener
    void setListener(ExpansionPort *port) { listener = port; }
};

#endif 
