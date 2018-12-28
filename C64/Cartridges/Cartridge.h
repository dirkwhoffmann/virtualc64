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
    
private:
    
    //
    // Cartridge configuration
    //
    
    /*! @brief    Initial value of the game line
     *  @details  The value is read from the CRT filt and the game line is set
     *            to it when the cartridge is plugged into the expansion port.
     */
    bool gameLineInCrtFile = 1;
    
    /*! @brief    Initial value of the exrom line
     *  @details  The value is read from the CRT filt and the exrom line is set
     *            to it when the cartridge is plugged into the expansion port.
     */
    bool exromLineInCrtFile = 1;
    
protected:
    
    //
    // ROM packets
    //
    
    //! @brief    Number of ROM packets
    uint8_t numPackets = 0;
    
    //! @brief    ROM chips contained in this cartridge
    CartridgeRom *packet[MAX_PACKETS];
    
    //! @brief    The ROM packet that is currently mapped to ROMx
    uint8_t chipL = 0;
    uint8_t chipH = 0;
    
    //! @brief    Number of bytes that are mapped to ROMx
    /*! @details  For most cartridges, this value is equals packet[romX]->size
     *            which means that the ROM is completely mapped.
     *            A value of 0 indicates that no ROM is currently mapped.
     */
    uint16_t mappedBytesL = 0;
    uint16_t mappedBytesH = 0;

    //! @brief    Offset into the ROM chip's data array
    /*! @details  The first ROMx byte has index offsetx
     *            The last ROMx byte has index  offsetx + mappedBytesx - 1
     */
    uint16_t offsetL = 0;
    uint16_t offsetH = 0;
    
private:
    
    //
    // On-board RAM
    //
    
    /*! @brief    Additional RAM
     *  @details  Some cartridges such as ActionReplay contain additional RAM.
     *            By default, this variable is NULL.
     */
    uint8_t *externalRam = NULL;
    
    /*! @brief    Capacity of the additional RAM in bytes
     *  @note     This value is 0 if and only if externaRam is NULL.
     */
    uint32_t ramCapacity = 0;
    
    //! @brief    Indicates if the RAM is kept alive during a reset.
    bool persistentRam = false;

    
    //
    // Hardware switches
    //
    
    /*! @brief    Current position of the cartridge switch (if any)
     *  @details  Only a cery few cartridges such as ISEPIC and EXPERT have
     *            a switch.
     */
    int8_t switchPos = 0;

    //! @brief    Status of the cartridge LED (true = on)
    bool led = false;
    
    
protected:
    
    //
    // Temporary storage (TODO: Move to custom cartridge classes)
    //
    
    /*! @brief    Temporary storage for cycle information
     *  @details  Some custom cartridges need to remember when certain event
     *            took place. When such an event happens, they preserve the
     *            cycle in this variable. Only a few cartridges make use of this
     *            variable.
     */
    // uint64_t cycle = 0;
    
    /*! @brief    Temporary storage
     *  @details  Some custom cartridges contain additonal registers or jumpers.
     *            They preserve these values in these general-purpose variables.
     *            Only a few cartridges make use of this variable.
     */
    uint8_t val[16]; 
    
public:
    
    //
    //! @functiongroup Class methods
    //
    
    /*! @brief    Checks the cartridge type.
     *  @details  Returns true iff the cartridge type is supported.
     */
    static bool isSupportedType(CartridgeType type);
    
    /*! @brief    Returns true if addr is located in the ROML address space.
     *  @details  If visible, ROML is always mapped to 0x8000 - 0x9FFF.
     */
    static bool isROMLaddr (uint16_t addr) { return addr >= 0x8000 && addr <= 0x9FFF; }
    
    /*! @brief    Returns true if addr is located in the ROMH address space.
     *  @details  ROMH can appear in 0xA000 - 0xBFFF or 0xE000 - 0xFFFF.
     */
    static bool isROMHaddr (uint16_t addr) {
        return (addr >= 0xA000 && addr <= 0xBFFF) || (addr >= 0xE000 && addr <= 0xFFFF); }

    
    //
    //! @functiongroup Creating and destructing
    //
    
    //! @brief    Convenience constructor
    Cartridge(C64 *c64, const char *description = "Cartridge");

    //! @brief    Destructor
    ~Cartridge();

    //! @brief    Deletes all chip packages
    void dealloc();

    //! @brief    Returns the cartridge type
    virtual CartridgeType getCartridgeType() { return CRT_NORMAL; }
    
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
    
    
    //
    //! @functiongroup Methods from VirtualComponent
    //
    
    void reset();
    void ping() { };
    size_t stateSize();
    void willLoadFromBuffer(uint8_t **buffer) { dealloc(); }
    void didLoadFromBuffer(uint8_t **buffer);
    void didSaveToBuffer(uint8_t **buffer);
    void dump();
    
    
    //
    //! @functiongroup Managing the cartridge configuration
    //
    
    //! @brief    Returns the initial state of the game line.
    bool getGameLineInCrtFile() { return gameLineInCrtFile; }
    
    //! @brief    Returns the initial state of the exrom line.
    bool getExromLineInCrtFile() { return exromLineInCrtFile; }
    
    /*! @brief    Resets the Game and Exrom line.
     *  @details  The default implementation resets the values to those found
     *            in the CRT file. Some custom cartridges need other start
     *            configurations and overwrite this function.
     */
    virtual void resetCartConfig();
    
    
    //
    //! @functiongroup Handling ROM packets
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
    //! @functiongroup Peeking and poking
    //
    
    /*! @brief    Peek fallthrough
     *  @param    addr must be a value in
     *            ROML range (0x8000 - 0x9FFF) or
     *            ROMH range (0xA000 - 0xBFFF, 0xE000 - 0xFFFF).
     */
    virtual uint8_t peek(uint16_t addr);

    /*! @brief    Peek fallthrough for the ROML space
     *  @param    addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual uint8_t peekRomL(uint16_t addr);
    
    /*! @brief    Peek fallthrough for the ROMH space
     *  @details  addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual uint8_t peekRomH(uint16_t addr);
    
    /*! @brief    Poke fallthrough
     *  @param    addr must be a value in
     *            ROML range (0x8000 - 0x9FFF) or
     *            ROMH range (0xA000 - 0xBFFF, 0xE000 - 0xFFFF).
     */
    virtual void poke(uint16_t addr, uint8_t value);

    /*! @brief    Poke fallthrough for the ROML space
     *  @param    addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual void pokeRomL(uint16_t addr, uint8_t value) { return; }
    
    /*! @brief    Poke fallthrough for the ROMH space
     *  @details  addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual void pokeRomH(uint16_t addr, uint8_t value) { return; }
    
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

    
    //
    //! @functiongroup Managing on-board RAM
    //
    
    //! @brief    Returns the RAM size in bytes.
    uint32_t getRamCapacity(); 

    //! @brief    Assigns external RAM to this cartridge.
    /*! @details  This functions frees any previously assigned RAM and allocates
     *            memory of the specified size. The size is stored in variable
     *            ramCapacity.
     */
    void setRamCapacity(uint32_t size);

    //! @brief    Returns true if RAM data is preserved during a reset.
    bool getPersistentRam() { return persistentRam; }

    //! @brief    Enables or disables persistent RAM.
    void setPersistentRam(bool value) { persistentRam = value; }

    //! @brief    Reads a byte from the on-board RAM.
    uint8_t peekRAM(uint16_t addr) {
        assert(addr < ramCapacity); return externalRam[addr]; }

    //! @brief    Writes a byte into the on-board RAM.
    void pokeRAM(uint16_t addr, uint8_t value) {
        assert(addr < ramCapacity); externalRam[addr] = value; }

    //! @brief    Erase the on-board RAM.
    void eraseRAM(uint8_t value) {
        assert(externalRam != NULL); memset(externalRam, value, ramCapacity); }
    

    //
    // Operating buttons
    //

    //! @brief    Returns the number of available cartridge buttons
    virtual unsigned numButtons() { return 0; }
    
    /*! @brief    Returns a textual description for a button.
     *  @return   NULL, if there is no such button.
     */
    virtual const char *getButtonTitle(unsigned nr) { return NULL; }
    
    /*! @brief    Presses a button
     *  @note     Make sure to call releaseButton() afterwards.
     */
    virtual void pressButton(unsigned nr) { }

    /*! @brief    Releases a button
     *  @note     Make sure to call pressButton() before.
     */
    virtual void releaseButton(unsigned nr) { }


    //
    // Operating switches
    //
    
    //! @brief    Returns true if the cartridge has a switch
    virtual bool hasSwitch() { return false; }

    //! @brief    Returns the current position of the switch
    virtual int8_t getSwitch() { return switchPos; }
    
    //! @brief    Convenience wrappers around getSwitch()
    bool switchIsNeutral() { return getSwitch() == 0; }
    bool switchIsLeft() { return getSwitch() < 0; }
    bool switchIsRight() { return getSwitch() > 0; }
    
    /*! @brief    Returns a textual description for a switch position.
     *  @return   NULL, if the switch cannot be positioned this way.
     */
    virtual const char *getSwitchDescription(int8_t pos) { return NULL; }
    
    //! @brief    Convenience wrappers around getSwitchDescription()
    bool validSwitchPosition(int8_t pos) { return getSwitchDescription(pos) != NULL; }
    
    //! @brief    Puts the switch in the provided position
    virtual void setSwitch(int8_t pos);

    
    //
    // Operating LEDs
    //
    
    //! @brief    Returns true if the cartridge has a LED.
    virtual bool hasLED() { return false; }
    
    //! @brief    Returns true if the LED is switched on.
    virtual bool getLED() { return led; }
    
    //! @brief    Switches the LED on or off.
    virtual void setLED(bool value) { led = value; }
    
    
    //
    // Delegation methods
    //
    
    //! @brief    Execution thread callback
    /*! @details  This function is invoked by the expansion port. Only a few
     *            cartridges such as EpyxFastLoader will do some action here.
     */
    virtual void execute() { };
    
    //! @brief    Modifies the memory source lookup tables if required
    virtual void updatePeekPokeLookupTables() { };
    
    //! @brief    Called when the C64 CPU triggers an NMI
    virtual void nmiWillTrigger() { }
    
    
    //
    // Little helpers
    //
    
    void resetWithoutDeletingRam();
};


/*!
 * @brief    Cartridge with an auxililary control register
 * @details  Many non-standard cartridges carry an addition register on board.
 */
class CartridgeWithRegister : public Cartridge {

protected:
    uint8_t control;
    
public:
    using Cartridge::Cartridge;
  
    void reset() {
        Cartridge::reset();
        control = 0;
    };
    size_t stateSize() {
        return Cartridge::stateSize() + 1;
    }
    void didLoadFromBuffer(uint8_t **buffer)
    {
        Cartridge::didLoadFromBuffer(buffer);
        control = read8(buffer);
    }
    void didSaveToBuffer(uint8_t **buffer)
    {
        Cartridge::didSaveToBuffer(buffer);
        write8(buffer, control);
    }
};


#endif 
