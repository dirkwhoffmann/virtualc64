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
// 4. Magic Desk, Domark, HES Australia, interesting test case

#ifndef _CARTRIDGE_INC
#define _CARTRIDGE_INC

#include "VirtualComponent.h"
#include "Cartridge_types.h"
#include "CRTFile.h"

class ExpansionPort;

/*!
 * @brief    Cartridge that can be plugged into the C64's expansion port
 */
class Cartridge : public VirtualComponent {
    
public:
    
    /*! @brief    Initial gameLine value used by this cartridge
     *  @details  The value is read from the CRT filt and the game line is set
     *            to it when the cartridge is plugged into the expansion port.
     */
    bool initialGameLine;
    
    /*! @brief    Initial exromLine value used by this cartridge
     *  @details  The value is read from the CRT filt and the exrom line is set
     *            to it when the cartridge is plugged into the expansion port.
     */
    bool initialExromLine;
    
    /*! @brief    ROM chips contained in the attached cartridge
     *  @details  A cartridge can contain up to 64 chips
     */
    uint8_t *chip[64];
    
    //! @brief    Array containing the load addresses of all chips
    uint16_t chipStartAddress[64];
    
    //! @brief    Array containing the chip sizes of all chips
    uint16_t chipSize[64];
    
    //! @brief    Number of the ROM chip that is currently mapped to ROMX
    /*! @details  -1 indicates that ROMX is unmapped.
     */
    int8_t chipL;
    int8_t chipH;

    //! @brief    Number of bytes that are mapped to ROMX
    /*! @details  For most cartridges, this value is equals chipSize[romX],
     *            which means that the ROM is completely mapped.
     */
    uint16_t mappedBytesL;
    uint16_t mappedBytesH;

    //! @brief    Offset into the ROM chip's data array
    /*! @details  The first ROMX byte is: chip[romX] + romOffsetX
     *            The last ROMX byte is: chip[romX] + romOffsetX + romSizeX - 1
     */
    uint16_t offsetL;
    uint16_t offsetH;

    /*! @brief    Indicates which ROM chip blended it
     *  @details  Each array item represents a 4 KB block above $8000
     *  @deprecated
     */
    uint8_t blendedIn[16];
    
    /*! @brief    Additional RAM
     *  @details  Some cartridges such as ActionReplay contain additional RAM.
     *            By default, this variable is NULL.
     */
    uint8_t *externalRam;
    
    /*! @brief    Capacity of the additional RAM in bytes
     *  @note     This value is 0 if and only if externaRam is NULL.
     */
    uint32_t ramCapacity; 
    
    //! @brief    Indicates if the RAM is kept alive during a reset.
    bool persistentRam;
    
    /*! @brief    Temporary storage for cycle information
     *  @details  Some custom cartridges need to remember when certain event
     *            took place. When such an event happens, they preserve the
     *            cycle in this variable. Only a few cartridges make use of this
     *            variable.
     */
    uint64_t cycle;
    
    /*! @brief    Temporary value storage
     *  @details  Some custom cartridges need to remember the last value that
     *            has been peeked or poked into the I/O registers. They preserve
     *            this value in this variable.
     */
    uint8_t regValue;

    
public:
    
    //! @brief    Convenience constructor
    Cartridge(C64 *c64);

    //! @brief    Destructor
    ~Cartridge();
    
    //! @brief    Check cartridge type
    /*! @details  Returns true iff the cartridge type is supported.
     */
    static bool isSupportedType(CartridgeType type);
    
    //! @brief    Factory method
    /*! @details  Creates a cartridge with the specified type. Make sure to pass
     *            containers of the supported cartridge type, only.
     *  @seealso  isSupportedType
     */
    static Cartridge *makeCartridgeWithType(C64 *c64, CartridgeType type);
    
    //! @brief    Factory method
    /*! @details  Creates a cartridge from a CRT container. Make sure to pass
     *            containers of the supported cartridge type, only.
     *  @seealso  isSupportedType
     */
    static Cartridge *makeCartridgeWithCRTContainer(C64 *c64, CRTFile *container);
    
    //! @brief    Methods from VirtualComponent
    void reset();
    void ping() { };
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    void dumpState();

    //! @brief    Indicates if CRT ROM is blended in at the specified address.
    bool romIsBlendedIn(uint16_t addr) { return blendedIn[addr >> 12]; }
    
    //! @brief    Execution thread callback
    /*! @details  This function is invoked by the expansion port. Only a few
     *            cartridges such as EpyxFastLoader will do some action here.
     */
    virtual void execute() { };
    
    //! @brief    Peek fallthrough
    //! @deprecated
    // virtual uint8_t peek(uint16_t addr);

    //! @brief    Peek fallthrough for the ROML space
    /*! @param    addr is the absolute address of the accessed memory cell.
     *            Valid range: 0x8000 - 0x9FFF
     */
    virtual uint8_t peekRomLabs(uint16_t absAddr);
    
    //! @brief    Peek fallthrough for the ROML space
    /*! @details  addr is the relative address of the accessed memory cell.
     *            Valid range: 0x0000 - 0x1FFF
     */
    virtual uint8_t peekRomL(uint16_t addr);
    
    //! @brief    Peek fallthrough for the ROMH space
    /*! @param    addr is the absolute address of the accessed memory cell.
     *            Valid range: 0xA000 - 0xBFFF and 0xE000 - 0xFFFF
     */
    virtual uint8_t peekRomHabs(uint16_t absAddr);
    
    //! @brief    Peek fallthrough for the ROMH space
    /*! @details  addr is the relative address of the accessed memory cell.
     *            Valid range: 0x0000 - 0x1FFF
     */
    virtual uint8_t peekRomH(uint16_t addr);
    
    //! @brief    Poke fallthrough
    //! @deprecated
    virtual void poke(uint16_t addr, uint8_t value) { return; }

    //! @brief    Same as peek, but without side effects.
    //! @deprecated
    // virtual uint8_t spypeek(uint16_t addr) { return peek(addr); }
    
    //! @brief    Same as peekRomL, but without side effects
    uint8_t spypeekRomLabs(uint16_t absAddr) { return peekRomLabs(absAddr); }
    uint8_t spypeekRomL(uint16_t addr) { return peekRomL(addr); }
    
    //! @brief    Same as peekRomH, but without side effects
    uint8_t spypeekRomHabs(uint16_t absAddr) { return peekRomHabs(absAddr); }
    uint8_t spypeekRomH(uint16_t addr) { return peekRomH(addr); }
    
    //! @brief    Peek fallthrough for I/O space 1
    virtual uint8_t peekIO1(uint16_t addr) { return 0; }

    //! @brief    Same as peekIO1, but without side effects.
    virtual uint8_t spypeekIO1(uint16_t addr) { return peekIO1(addr); }

    //! @brief    Peek fallthrough for I/O space 2
    virtual uint8_t peekIO2(uint16_t addr) { return 0; }

    //! @brief    Same as peekIO2, but without side effects.
    virtual uint8_t spypeekIO2(uint16_t addr) { return peekIO2(addr); }
    
    //! @brief    Poke fallthrough for I/O space 1
    virtual void pokeIO1(uint16_t addr, uint8_t value) { }

    //! @brief    Poke fallthrough for I/O space 2
    virtual void pokeIO2(uint16_t addr, uint8_t value) { }

    //! @brief    Returns the cartridge type
    virtual CartridgeType getCartridgeType() { return CRT_NORMAL; }
    
    /*! @brief    Counts the number of chips
     *  @return   Value between 0 and 64
     */
    unsigned numberOfChips();
    
    //! @brief    Sums up the sizes of all chips in bytes
    unsigned numberOfBytes();
    
    //! @brief    Returns the RAM size in bytes.
    uint32_t getRamCapacity(); 

    //! @brief    Assigns external RAM to this cartridge.
    /*! @details  This functions frees any previously assigned RAM and allocates
     *            memory of the specified size. The size is stored in variable
     *            ramCapacity.
     */
    void setRamCapacity(uint32_t size);

    //! @brief    Returns the initial state of the game line.
    bool getInitialGameLine() { return initialGameLine; }
        
    //! @brief    Returns the initial state of the exrom line.
    bool getInitialExromLine() { return initialExromLine; }
    
    //! @brief    Returns true if a certain ROM chip maps to ROML, only.
    bool mapsToL(unsigned nr);

    //! @brief    Returns true if a certain ROM chip maps to ROML and ROMH.
    bool mapsToLH(unsigned nr);

    //! @brief    Returns true if a certain ROM chip maps to ROMH, only.
    bool mapsToH(unsigned nr);

    //! @brief    Banks in a chip
    /*! @details  Chip contents will show up in memory
     */
    void bankIn(unsigned nr);
    
    //! @brief    Banks in a chip
    /*! @details  RAM contents will show in memory
     */
    void bankOut(unsigned nr); // Deprecated. Will be deleted
    void bankOutNew(unsigned nr); // Rename to bankIn when stable

    //! @brief    Reads in chip stored in the provided CRT container
    void loadChip(unsigned nr, CRTFile *c);    

    //! @brief    Simulates pressing a cartridge button
    /*! @details  By default nothing is done here as most cartridges do not
     *            have any button. Some special cartriges such aus Final
     *            Cartridge III overwrite this function to emulate a freezer
     *            button.
     */
    virtual void pressFirstButton() { };
    virtual void pressSecondButton() { };
    
    //! @brief    Simulates releasing a cartridge button
    virtual void releaseFirstButton() { };
    virtual void releaseSecondButton() { };
};

#endif 
