// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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

#include "C64Component.h"
#include "Cartridge.h"
// #include "ExpansionPortTypes.h"

class ExpansionPort : public C64Component {
 
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
    
    
    //
    // Constructing and serializing
    //
    
public:
    
    ExpansionPort(C64 &ref);
    ~ExpansionPort();
    
    
    //
    // Methods from HardwareComponent
    //
    
public:
    
    void _reset() override;
    size_t oldStateSize() override;
    void oldDidLoadFromBuffer(u8 **buffer) override;
    void oldDidSaveToBuffer(u8 **buffer) override;

private:
    
    void _ping() override;
    void _dump() override;

    
public:
    
    //! @brief    Execution thread callback
    /*! @details  This method is invoked after each rasterline.
     */
    void execute() { if (cartridge) cartridge->execute(); }
    
    //! @brief    Peek fallthrough
    u8 peek(u16 addr);
    
    //! @brief    Same as peek, but without side effects
    u8 spypeek(u16 addr);
    
    //! @brief    Peek fallthrough for I/O space 1
    u8 peekIO1(u16 addr);
    
    //! @brief    Same as peekIO1, but without side effects
    u8 spypeekIO1(u16 addr);
    
    //! @brief    Peek fallthrough for I/O space 2
    u8 peekIO2(u16 addr);

    //! @brief    Same as peekIO2, but without side effects
    u8 spypeekIO2(u16 addr);
    
    //! @brief    Poke fallthrough
    void poke(u16 addr, u8 value);
    
    //! @brief    Poke fallthrough for I/O space 1
    void pokeIO1(u16 addr, u8 value);
    
    //! @brief    Poke fallthrough for I/O space 2
    void pokeIO2(u16 addr, u8 value);
    
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
    bool attachGeoRamCartridge(u32 capacity);

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
   
    //! @brief    Returns true if a cartridge with a switch is attached
    bool hasSwitch() { return cartridge ? cartridge->hasSwitch() : false; }
    
    //! @brief    Returns the current position of the switch
    i8 getSwitch() { return cartridge ? cartridge->getSwitch() : 0; }
    bool switchIsNeutral() { return cartridge ? cartridge->switchIsNeutral() : false; }
    bool switchIsLeft() { return cartridge ? cartridge->switchIsLeft() : false; }
    bool switchIsRight() { return cartridge ? cartridge->switchIsRight() : false; }
    const char *getSwitchDescription(i8 pos) {
        return cartridge ? cartridge->getSwitchDescription(pos) : NULL; }
    const char *getSwitchDescription() {
        return getSwitchDescription(getSwitch()); }
    bool validSwitchPosition(i8 pos) {
        return cartridge ? cartridge->validSwitchPosition(pos) : false; }
    
    //! @brief    Puts the switch in the provided position
    void setSwitch(u8 pos) { if (cartridge) cartridge->setSwitch(pos); }

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
    
    //! @brief    Called when the C64 CPU is about to trigger an NMI
    void nmiWillTrigger() { if (cartridge) cartridge->nmiWillTrigger(); }

    //! @brief    Called after the C64 CPU has processed the NMI instruction
    void nmiDidTrigger() { if (cartridge) cartridge->nmiDidTrigger(); }

};
    
#endif
