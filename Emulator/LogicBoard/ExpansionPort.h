// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ExpansionPortTypes.h"
#include "SubComponent.h"
#include "Cartridge.h"

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

class ExpansionPort : public SubComponent {
    
    // Attached cartridge
    std::unique_ptr<Cartridge> cartridge;
    
    // Type of the attached cartridge
    CartridgeType crtType = CRT_NONE;
    
    // Values of the Game and the Exrom line (true if no cartridge is attached)
    bool gameLine = 1;
    bool exromLine = 1;
    
    
    //
    // Initializing
    //
    
public:
    
    ExpansionPort(C64 &ref) : SubComponent(ref) { };
    ~ExpansionPort();
    const char *getDescription() const override { return "ExpansionPort"; }

private:
    
    void _reset(bool hard) override;
    
    
    //
    // Analyzing
    //
    
public:
    
    CartridgeType getCartridgeType();

private:
    
    void _dump(dump::Category category, std::ostream& os) const override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << crtType
        << gameLine
        << exromLine;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }
    
    isize _size() override;
    isize _load(const u8 *buffer) override;
    isize _save(u8 *buffer) override;

 
    //
    // Accessing cartrige memory
    //
    
public:
    
    u8 peek(u16 addr);
    u8 spypeek(u16 addr) const;
    u8 peekIO1(u16 addr);
    u8 spypeekIO1(u16 addr) const;
    u8 peekIO2(u16 addr);
    u8 spypeekIO2(u16 addr) const;
    
    void poke(u16 addr, u8 value);
    void pokeIO1(u16 addr, u8 value);
    void pokeIO2(u16 addr, u8 value);
    
    
    //
    // Controlling the Game and Exrom lines
    //
    
public:
    
    bool getGameLine() const { return gameLine; }
    void setGameLine(bool value);
    
    bool getExromLine() const { return exromLine; }
    void setExromLine(bool value);
    
    void setGameAndExrom(bool game, bool exrom);
    
    CRTMode getCartridgeMode() const;
    void setCartridgeMode(CRTMode mode);

    
    //
    // Attaching and detaching
    //
    
    // Returns true if a cartridge is attached to the expansion port
    bool getCartridgeAttached() const { return cartridge != nullptr; }

    // Attaches a cartridge to the expansion port
    void attachCartridge(const string &path, bool reset = true) throws;
    void attachCartridge(CRTFile *c, bool reset = true) throws;
    void attachCartridge(Cartridge *c);
    void attachGeoRamCartridge(isize capacity);
    void attachIsepicCartridge();

    // Removes a cartridge from the expansion port (if any)
    void detachCartridge();
    void detachCartridgeAndReset();

    
    //
    // Managing on-board RAM
    //
    
    // Returns the size of the on-board RAM in bytes
    isize getRamCapacity() const;
    
    // Returns true if the attached cartridge has a RAM backing battery
    bool hasBattery() const;

    // Enables or disables RAM backing during a reset
    void setBattery(bool value);
    
    
    //
    // Operating buttons
    //
    
    // Returns the number of available cartridge buttons
    isize numButtons() const;
    
    // Returns a textual description for a button
    const string getButtonTitle(isize nr) const;
    
    // Presses a button (make sure to call releaseButton() afterwards)
    void pressButton(isize nr);
    
    // Releases a button (make sure to call pressButton() before)
    void releaseButton(isize nr);
   
    
    //
    // Operating switches
    //
    
    // Returns true if the cartridge has a switch
    bool hasSwitch() const;
    
    // Returns the current switch position
    i8 getSwitch() const;
    bool switchIsNeutral() const;
    bool switchIsLeft() const;
    bool switchIsRight() const;
    
    /* Returns a textual description for a switch position or nullptr if the
     * switch cannot be positioned this way.
     */
    const string getSwitchDescription(i8 pos) const;
    const string getSwitchDescription() const;
    bool validSwitchPosition(i8 pos) const;
    
    // Puts the switch in the provided position
    void setSwitch(u8 pos) { if (cartridge) cartridge->setSwitch(pos); }

    
    //
    // Operating LEDs
    //
    
    // Returns true if the cartridge has a LED
    bool hasLED() const;
    
    // Returns true if the LED is switched on
    bool getLED() const;
    
    // Switches the LED on or off
    void setLED(bool value);
    
    
    //
    // Handling delegation calls
    //
    
    /* Emulator thread callback. This function is invoked by the expansion port.
     * Only a few cartridges such as EpyxFastLoader will do some action here.
     */
    void execute();
    
    /* Modifies the memory source lookup tables if required. This function is
     * called in C64::updatePeekPokeLookupTables() to allow cartridges to
     * manipulate the lookup tables after the default values have been set.
     * Background: Some cartridges such as StarDos change the game and exrom
     * line on-the-fly to achieve very special memory mappings. For most
     * cartridges, this function does nothing.
     */
    void updatePeekPokeLookupTables();
    
    // Called when the C64 CPU is about to trigger an NMI
    void nmiWillTrigger();

    // Called after the C64 CPU has processed the NMI instruction
    void nmiDidTrigger();
};
 
