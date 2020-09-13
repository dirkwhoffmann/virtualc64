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

class Cartridge : public C64Component {
    
    //
    // Constants
    //
    
public:
    
    // Maximum number of chip packets on a single cartridge
    static const unsigned MAX_PACKETS = 128;
    
    
    //
    // Cartridge configuration
    //

private:

    /* Initial values of the game and exrom lines. The values are read from the
     * CRT file and the game line is set to it when the cartridge is plugged
     * into the expansion port.
     */
    bool gameLineInCrtFile = 1;
    bool exromLineInCrtFile = 1;
    
    
    //
    // Rom packets
    //
    
protected:

    u8 numPackets = 0;
    CartridgeRom *packet[MAX_PACKETS];
    
    // Indicates which packets are currently mapped to ROML and ROMH
    u8 chipL = 0;
    u8 chipH = 0;
    
    /* Number of bytes that are mapped to ROML and ROMH. For most cartridges,
     * this value is equals packet[romX]->size which means that the ROM is
     * completely mapped. A value of 0 indicates that no ROM is currently
     * mapped.
     */
    u16 mappedBytesL = 0;
    u16 mappedBytesH = 0;

    /* Offset into the ROM chip's data array. The first ROMx byte has index
     * offsetx. The last ROMx byte has index offsetx + mappedBytesx - 1.
     */
    u16 offsetL = 0;
    u16 offsetH = 0;
    
private:
    
    //
    // On-board RAM
    //
    
    /* Additional RAM. Some cartridges such as ActionReplay contain additional
     * RAM. By default, this variable is NULL.
     */
    u8 *externalRam = NULL;
    
    /* Capacity of the additional RAM in bytes. This value is 0 if and only if
     * externaRam is NULL.
     */
    u32 ramCapacity = 0;
    
    // Indicates if the RAM contents is preserved during a reset
    bool battery = false;

    
    //
    // On-board registers
    //
    
protected:
    
    /* Auxililary control register. Many non-standard cartridges carry an
     * additional register on board.
     */
    u8 control = 0;
        
    
    //
    // Hardware switches
    //
    
protected:
    
    /* Current position of the cartridge switch (if any). Only a few cartridges
     * have a switch such as ISEPIC and EXPERT.
     */
    i8 switchPos = 0;

    // Status of the cartridge LED (true = on)
    bool led = false;
    

    //
    // Class methods
    //

public:
        
    // Checks whether this cartridge is a supported by the emulator
    static bool isSupportedType(CartridgeType type);
    
    // Returns true if addr is located in the ROML or the ROMH address space
    static bool isROMLaddr (u16 addr);
    static bool isROMHaddr (u16 addr);

    // Factory methods
    static Cartridge *makeWithType(C64 &c64, CartridgeType type);
    static Cartridge *makeWithCRTFile(C64 &c64, CRTFile *file);

    
    //
    // Initializing
    //
    
public:
    
    Cartridge(C64 &ref, const char *description = "Cartridge");
    ~Cartridge();
    
    /* Resets the Game and the Exrom line. The default implementation resets
     * the values to ones found in the CRT file. A few custom cartridges need
     * other start configurations and overwrite this function.
     */
    virtual void resetCartConfig();

protected:
    
    void dealloc();
    void _reset() override;
    void resetWithoutDeletingRam();
        
    
    //
    // Analyzing
    //

public:
    
    virtual CartridgeType getCartridgeType() { return CRT_NORMAL; }

protected:
    
    void _dump() override;
        
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & gameLineInCrtFile
        & exromLineInCrtFile
        & numPackets
        & ramCapacity
        & battery
        & control
        & switchPos;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & chipL
        & chipH
        & mappedBytesL
        & mappedBytesH
        & offsetL
        & offsetH
        & led;
    }
    
protected:
    
    size_t _size() override;
    size_t _load(u8 *buffer) override;
    size_t _save(u8 *buffer) override;
        
        
    //
    // Accessing
    //
    
public:
    
    // Returns the initial value of the Game or the Exrom line
    bool getGameLineInCrtFile() { return gameLineInCrtFile; }
    bool getExromLineInCrtFile() { return exromLineInCrtFile; }
        
    
    //
    // Handling ROM packets
    //
    
    // Reads in a chip packet from a CRT file
    virtual void loadChip(unsigned nr, CRTFile *c);
    
    // Banks in a rom chip into the ROML or the ROMH space
    void bankInROML(unsigned nr, u16 size, u16 offset);
    void bankInROMH(unsigned nr, u16 size, u16 offset);
    
    /* Banks in a rom chip. This function calls bankInROML or bankInROMH with
     * the default parameters for this chip as provided in the CRT file.
     */
    void bankIn(unsigned nr);
    
    //  Banks out a chip (RAM will be visible again)
    void bankOut(unsigned nr);

    
    //
    // Accessing cartridge memory
    //
    
    /* Fallthroughs for the cartridge memory
     *
     *     ROML range: 0x8000 - 0x9FFF
     *     ROMH range: 0xA000 - 0xBFFF and 0xE000 - 0xFFFF
     */
    virtual u8 peek(u16 addr);
    virtual u8 peekRomL(u16 addr);
    virtual u8 peekRomH(u16 addr);

    virtual u8 spypeek(u16 addr) { return peek(addr); }
    virtual u8 spypeekRomL(u16 addr) { return peekRomL(addr); }
    virtual u8 spypeekRomH(u16 addr) { return peekRomH(addr); }

    virtual void poke(u16 addr, u8 value);
    virtual void pokeRomL(u16 addr, u8 value) { return; }
    virtual void pokeRomH(u16 addr, u8 value) { return; }
    
    // Fallthroughs for the I/O spaces
    virtual u8 peekIO1(u16 addr) { return 0; }
    virtual u8 peekIO2(u16 addr) { return 0; }

    virtual u8 spypeekIO1(u16 addr) { return peekIO1(addr); }
    virtual u8 spypeekIO2(u16 addr) { return peekIO2(addr); }
    
    virtual void pokeIO1(u16 addr, u8 value) { }
    virtual void pokeIO2(u16 addr, u8 value) { }

    
    //
    // Managing on-board RAM
    //
    
    // Returns the RAM size in bytes
    u32 getRamCapacity(); 

    /* Assigns external RAM to this cartridge. This functions frees any
     * previously assigned RAM and allocates memory of the specified size. The
     * size is stored in variable ramCapacity.
     */
    void setRamCapacity(u32 size);

    // Returns true if RAM data is preserved during a reset
    bool getBattery() { return battery; }

    // Enables or disables persistent RAM
    void setBattery(bool value) { battery = value; }

    // Reads or write RAM cells
    u8 peekRAM(u16 addr);
    void pokeRAM(u16 addr, u8 value);
    void eraseRAM(u8 value);

    
    //
    // Operating buttons
    //

    // Returns the number of available cartridge buttons
    virtual long numButtons() { return 0; }
    
    /* Returns a textual description for a button or NULL, if there is no
     * button with the specified number.
     */
    virtual const char *getButtonTitle(unsigned nr) { return NULL; }
    
    // Presses a button (make sure to call releaseButton() afterwards)
    virtual void pressButton(unsigned nr) { }

    // Releases a button (make sure to call pressButton() before)
    virtual void releaseButton(unsigned nr) { }


    //
    // Operating switches
    //
    
    // Returns true if the cartridge has a switch
    virtual bool hasSwitch() { return false; }

    // Returns the current switch position
    virtual i8 getSwitch() { return switchPos; }
    bool switchIsNeutral() { return getSwitch() == 0; }
    bool switchIsLeft() { return getSwitch() < 0; }
    bool switchIsRight() { return getSwitch() > 0; }
    
    /* Returns a textual description for a switch position or NULL if the
     * switch cannot be positioned this way.
     */
    virtual const char *getSwitchDescription(i8 pos) { return NULL; }
    const char *getSwitchDescription() { return getSwitchDescription(getSwitch()); }
    bool validSwitchPosition(i8 pos) { return getSwitchDescription(pos) != NULL; }
    
    // Puts the switch in a certain position
    virtual void setSwitch(i8 pos);

    
    //
    // Operating LEDs
    //
    
    // Returns true if the cartridge has a LED
    virtual bool hasLED() { return false; }
    
    // Returns true if the LED is switched on
    virtual bool getLED() { return led; }
    
    // Switches the LED on or off
    virtual void setLED(bool value) { led = value; }
    
    
    //
    // Handling delegation calls
    //
    
    /* Emulator thread callback. This function is invoked by the expansion port.
     * Only a few cartridges such as EpyxFastLoader will do some action here.
     */
    virtual void execute() { };
    
    // Modifies the memory source lookup tables if required
    virtual void updatePeekPokeLookupTables() { };
    
    // Called when the C64 CPU is about to trigger an NMI
    virtual void nmiWillTrigger() { }
    
    // Called after the C64 CPU has processed the NMI instruction
    virtual void nmiDidTrigger() { }
};

#endif 
