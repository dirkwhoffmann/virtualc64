/*!
 * @header      Cartridge.h
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


#ifndef _CARTRIDGE_INC
#define _CARTRIDGE_INC

#include "VirtualComponent.h"
#include "Cartridge_types.h"
#include "CRTFile.h"
#include "CartridgeRom.h"

class ExpansionPort;

/*!
 * @brief    Cartridge that can be plugged into the C64's expansion port
 */
class Cartridge : public VirtualComponent {
    
    public:
    
    //! @brief    Maximum number of chip packets on a single cartridge.
    static const unsigned MAX_PACKETS = 128;
    
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
    
    //! @brief    Number of ROM packets
    uint8_t numPackets;
    
    //! @brief    ROM chips contained in this cartridge
    CartridgeRom *packet[MAX_PACKETS];
    
    //! @brief    Number of the ROM chip that is currently mapped to ROMx
    uint8_t chipL, chipH;

    //! @brief    Number of bytes that are mapped to ROMX
    /*! @details  For most cartridges, this value is equals packet[romX]->size
     *            which means that the ROM is completely mapped.
     *            A value of 0 indicates that no ROM is currently mapped.
     */
    uint16_t mappedBytesL, mappedBytesH;

    //! @brief    Offset into the ROM chip's data array
    /*! @details  The first ROMX byte is: chip[romX] + romOffsetX
     *            The last ROMX byte is: chip[romX] + romOffsetX + romSizeX - 1
     */
    uint16_t offsetL, offsetH;
    
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
    
    /*! @brief    Temporary storage
     *  @details  Some custom cartridges contain additonal registers or jumpers.
     *            They preserve these values in these general-purpose variables.
     *            Only a few cartridges make use of this variable.
     */
    uint8_t val[16];
    
    /*! @brief    Temporary value storage
     *  @details  Some custom cartridges need to remember the last value that
     *            has been peeked or poked into the I/O registers. They preserve
     *            this value in this variable.
     *  @deprecated Use val[] instead
     */
    uint8_t regValue;

    
public:
    
    //
    //! @functiongroup Class methods
    //
    
    /*! @brief    Returns true if addr is located in the ROML address space
     *  @details  If visible, ROML is always mapped to 0x8000 - 0x9FFF.
     */
    static bool isROMLaddr (uint16_t addr) { return addr >= 0x8000 && addr <= 0x9FFF; }
    
    /*! @brief    Returns true if addr is located in the ROMH address space
     *  @details  ROMH can appear in 0xA000 - 0xBFFF or 0xE000 - 0xFFFF.
     */
    static bool isROMHaddr (uint16_t addr) {
        return (addr >= 0xA000 && addr <= 0xBFFF) || (addr >= 0xE000 && addr <= 0xFFFF); }
    
    /*! @brief    Check cartridge type
     *  @details  Returns true iff the cartridge type is supported.
     */
    static bool isSupportedType(CartridgeType type);
    
    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Convenience constructor
    Cartridge(C64 *c64);

    //! @brief    Destructor
    ~Cartridge();

    //! @brief    Deletes all chip packages
    void dealloc();

    //! @brief    Factory method
    /*! @details  Creates a cartridge with the specified type. Make sure to pass
     *            containers of the supported cartridge type, only.
     *  @seealso  isSupportedType
     */
    static Cartridge *makeWithType(C64 *c64, CartridgeType type);
    
    //! @brief    Factory method
    /*! @details  Creates a cartridge from a CRT file. Make sure to pass
     *            containers of the supported cartridge type, only.
     *  @seealso  isSupportedType
     */
    static Cartridge *makeWithCRTFile(C64 *c64, CRTFile *file);
    
    //! @brief    State size function for chip packet data
    virtual size_t packetStateSize();
    
    //! @brief    Loads all chip packets from a buffer
    virtual void loadPacketsFromBuffer(uint8_t **buffer);
    
    //! @brief    Saves all chip packets to a buffer
    virtual void savePacketsToBuffer(uint8_t **buffer);
    
    //! @brief    Methods from VirtualComponent
    void reset();
    void ping() { };
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    void dump();
    
    //! @brief    Execution thread callback
    /*! @details  This function is invoked by the expansion port. Only a few
     *            cartridges such as EpyxFastLoader will do some action here.
     */
    virtual void execute() { };
    
    //! @brief    Peek fallthrough
    /*! @param    addr must be a value in the ROML range (0x8000 - 0x9FFF) or
     *            the ROMH range (0xA000 - 0xBFFF, 0xE000 - 0xFFFF).
     */
    virtual uint8_t peek(uint16_t addr);

    //! @brief    Peek fallthrough for the ROML space
    /*! @param    addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual uint8_t peekRomL(uint16_t addr);
    
    //! @brief    Peek fallthrough for the ROMH space
    /*! @details  addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual uint8_t peekRomH(uint16_t addr);
    
    //! @brief    Poke fallthrough
    virtual void poke(uint16_t addr, uint8_t value) { return; }

    //! @brief    Same as peek, but without side effects.
    virtual uint8_t spypeek(uint16_t addr) { return peek(addr); }
    
    //! @brief    Same as peekRomL, but without side effects
    uint8_t spypeekRomL(uint16_t addr) { return peekRomL(addr); }
    
    //! @brief    Same as peekRomH, but without side effects
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
    
    
    //
    //! @functiongroup ROM chip handling
    //
    
    //! @brief    Reads in a chip packet from a CRT file
    virtual void loadChip(unsigned nr, CRTFile *c);

    //! @brief    Banks in a rom chip into the ROML space
    void bankInROML(unsigned nr, uint16_t size, uint16_t offset);

    //! @brief    Banks in a rom chip into the ROMH space
    void bankInROMH(unsigned nr, uint16_t size, uint16_t offset);

    //! @brief    Banks in a rom chip
    /*! @details  This function calls bankInROML or bankInROMH with the default
     *            parameters for this chip as provided in the CRT file.
     */
    void bankIn(unsigned nr);
    
    //! @brief    Banks out a chip
    /*! @details  RAM contents will show in memory
     */
    void bankOut(unsigned nr);


    //
    // Cartridge buttons
    //

    //! @brief    Returns true if the cartridge has a freeze button
    virtual bool hasFreezeButton() { return false; }

    //! @brief    Simulates pressing the freeze cartridge button (if present)
    /*! @note     Make sure to call releaseFreezeButton() afterwards.
     *  @seealso  releaseFreezeButton, hasFreezeButton
     */
    virtual void pressFreezeButton() { };

    //! @brief    Simulates releasing the freeze cartridge button (if present)
    /*! @note     Make sure to call pressFreezeButton() first.
     *  @seealso  pressFreezeButton, hasFreezeButton
     */
    virtual void releaseFreezeButton() { };
    
    //! @brief    Returns true if the cartridge has a reset button
    virtual bool hasResetButton() { return false; }
    
    //! @brief    Simulates pressing the reset cartridge button (if present)
    /*! @note     Make sure to call releaseResetButton() afterwards.
     *  @seealso  releaseResetButton
     */
    virtual void pressResetButton();
    
    //! @brief    Simulates releasing the reset cartridge button (if present)
    /*! @note     Make sure to call pressResetButton() first.
     *  @seealso  pressSecondButton
     */
    virtual void releaseResetButton() { };
};

#endif 
