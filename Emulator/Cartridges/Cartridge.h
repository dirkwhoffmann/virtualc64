// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include "C64Component.h"
#include "CartridgeTypes.h"
#include "CRTFile.h"
#include "CartridgeRom.h"

// class ExpansionPort;

class Cartridge : public C64Component {
    
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
    u8 numPackets = 0;
    
    //! @brief    ROM chips contained in this cartridge
    CartridgeRom *packet[MAX_PACKETS];
    
    //! @brief    The ROM packet that is currently mapped to ROMx
    u8 chipL = 0;
    u8 chipH = 0;
    
    //! @brief    Number of bytes that are mapped to ROMx
    /*! @details  For most cartridges, this value is equals packet[romX]->size
     *            which means that the ROM is completely mapped.
     *            A value of 0 indicates that no ROM is currently mapped.
     */
    u16 mappedBytesL = 0;
    u16 mappedBytesH = 0;

    //! @brief    Offset into the ROM chip's data array
    /*! @details  The first ROMx byte has index offsetx
     *            The last ROMx byte has index  offsetx + mappedBytesx - 1
     */
    u16 offsetL = 0;
    u16 offsetH = 0;
    
private:
    
    //
    // On-board RAM
    //
    
    /*! @brief    Additional RAM
     *  @details  Some cartridges such as ActionReplay contain additional RAM.
     *            By default, this variable is NULL.
     */
    u8 *externalRam = NULL;
    
    /*! @brief    Capacity of the additional RAM in bytes
     *  @note     This value is 0 if and only if externaRam is NULL.
     */
    u32 ramCapacity = 0;
    
    //! @brief    Indicates if the RAM is kept alive during a reset.
    bool persistentRam = false;

    
    //
    // Hardware switches
    //
    
    /*! @brief    Current position of the cartridge switch (if any)
     *  @details  Only a cery few cartridges such as ISEPIC and EXPERT have
     *            a switch.
     */
    i8 switchPos = 0;

    //! @brief    Status of the cartridge LED (true = on)
    bool led = false;
    
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
    static bool isROMLaddr (u16 addr) { return addr >= 0x8000 && addr <= 0x9FFF; }
    
    /*! @brief    Returns true if addr is located in the ROMH address space.
     *  @details  ROMH can appear in 0xA000 - 0xBFFF or 0xE000 - 0xFFFF.
     */
    static bool isROMHaddr (u16 addr) {
        return (addr >= 0xA000 && addr <= 0xBFFF) || (addr >= 0xE000 && addr <= 0xFFFF); }

    
    //
    // Constructing and serializing
    //
    
    Cartridge(C64 *c64, C64 &ref, const char *description = "Cartridge");
    ~Cartridge();

    // Deletes all chip packages
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
    virtual void loadPacketsFromBuffer(u8 **buffer);
    
    //! @brief    Saves all chip packets to a buffer
    virtual void savePacketsToBuffer(u8 **buffer);
    
    
    //
    // Methods from HardwareComponent
    //
    
public:
    
    void reset() override;
    void ping() override { };
    size_t stateSize() override;
    void willLoadFromBuffer(u8 **buffer) override { dealloc(); }
    void didLoadFromBuffer(u8 **buffer) override;
    void didSaveToBuffer(u8 **buffer) override;

private:

    void _dump() override;
    
        
    //
    //! @functiongroup Managing the cartridge configuration
    //
    
public:
    
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
    void bankInROML(unsigned nr, u16 size, u16 offset);
    
    //! @brief    Banks in a rom chip into the ROMH space
    void bankInROMH(unsigned nr, u16 size, u16 offset);
    
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
    virtual u8 peek(u16 addr);

    /*! @brief    Peek fallthrough for the ROML space
     *  @param    addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual u8 peekRomL(u16 addr);
    
    /*! @brief    Peek fallthrough for the ROMH space
     *  @details  addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual u8 peekRomH(u16 addr);
    
    /*! @brief    Poke fallthrough
     *  @param    addr must be a value in
     *            ROML range (0x8000 - 0x9FFF) or
     *            ROMH range (0xA000 - 0xBFFF, 0xE000 - 0xFFFF).
     */
    virtual void poke(u16 addr, u8 value);

    /*! @brief    Poke fallthrough for the ROML space
     *  @param    addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual void pokeRomL(u16 addr, u8 value) { return; }
    
    /*! @brief    Poke fallthrough for the ROMH space
     *  @details  addr must be a value between 0x0000 - 0x1FFF.
     */
    virtual void pokeRomH(u16 addr, u8 value) { return; }
    
    //! @brief    Same as peek, but without side effects.
    virtual u8 spypeek(u16 addr) { return peek(addr); }
    
    //! @brief    Same as peekRomL, but without side effects
    u8 spypeekRomL(u16 addr) { return peekRomL(addr); }
    
    //! @brief    Same as peekRomH, but without side effects
    u8 spypeekRomH(u16 addr) { return peekRomH(addr); }
    
    //! @brief    Peek fallthrough for I/O space 1
    virtual u8 peekIO1(u16 addr) { return 0; }

    //! @brief    Same as peekIO1, but without side effects.
    virtual u8 spypeekIO1(u16 addr) { return peekIO1(addr); }

    //! @brief    Peek fallthrough for I/O space 2
    virtual u8 peekIO2(u16 addr) { return 0; }

    //! @brief    Same as peekIO2, but without side effects.
    virtual u8 spypeekIO2(u16 addr) { return peekIO2(addr); }
    
    //! @brief    Poke fallthrough for I/O space 1
    virtual void pokeIO1(u16 addr, u8 value) { }

    //! @brief    Poke fallthrough for I/O space 2
    virtual void pokeIO2(u16 addr, u8 value) { }

    
    //
    //! @functiongroup Managing on-board RAM
    //
    
    //! @brief    Returns the RAM size in bytes.
    u32 getRamCapacity(); 

    //! @brief    Assigns external RAM to this cartridge.
    /*! @details  This functions frees any previously assigned RAM and allocates
     *            memory of the specified size. The size is stored in variable
     *            ramCapacity.
     */
    void setRamCapacity(u32 size);

    //! @brief    Returns true if RAM data is preserved during a reset.
    bool getPersistentRam() { return persistentRam; }

    //! @brief    Enables or disables persistent RAM.
    void setPersistentRam(bool value) { persistentRam = value; }

    //! @brief    Reads a byte from the on-board RAM.
    u8 peekRAM(u16 addr) {
        assert(addr < ramCapacity); return externalRam[addr]; }

    //! @brief    Writes a byte into the on-board RAM.
    void pokeRAM(u16 addr, u8 value) {
        assert(addr < ramCapacity); externalRam[addr] = value; }

    //! @brief    Erase the on-board RAM.
    void eraseRAM(u8 value) {
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
    virtual i8 getSwitch() { return switchPos; }
    
    //! @brief    Convenience wrappers around getSwitch()
    bool switchIsNeutral() { return getSwitch() == 0; }
    bool switchIsLeft() { return getSwitch() < 0; }
    bool switchIsRight() { return getSwitch() > 0; }
    
    /*! @brief    Returns a textual description for a switch position.
     *  @return   NULL, if the switch cannot be positioned this way.
     */
    virtual const char *getSwitchDescription(i8 pos) { return NULL; }
    
    //! @brief    Convenience wrappers around getSwitchDescription()
    bool validSwitchPosition(i8 pos) { return getSwitchDescription(pos) != NULL; }
    
    //! @brief    Puts the switch in the provided position
    virtual void setSwitch(i8 pos);

    
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
    
    //! @brief    Called when the C64 CPU is about to trigger an NMI
    virtual void nmiWillTrigger() { }
    
    //! @brief    Called after the C64 CPU has processed the NMI instruction
    virtual void nmiDidTrigger() { }
    
    
    //
    // Helpers
    //
    
    void resetWithoutDeletingRam();
};


/*!
 * @brief    Cartridge with an auxililary control register
 * @details  Many non-standard cartridges carry an addition register on board.
 */
class CartridgeWithRegister : public Cartridge {

protected:
    u8 control;
    
public:
    using Cartridge::Cartridge;
  
    void reset() {
        Cartridge::reset();
        control = 0;
    }
    size_t stateSize() {
        return Cartridge::stateSize() + 1;
    }
    void didLoadFromBuffer(u8 **buffer)
    {
        Cartridge::didLoadFromBuffer(buffer);
        control = read8(buffer);
    }
    void didSaveToBuffer(u8 **buffer)
    {
        Cartridge::didSaveToBuffer(buffer);
        write8(buffer, control);
    }
};

#endif 
