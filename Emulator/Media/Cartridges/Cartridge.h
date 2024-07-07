// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "CartridgeTypes.h"
#include "SubComponent.h"
#include "CartridgeRom.h"
#include "CRTFile.h"

namespace vc64 {

class Cartridge : public SubComponent, public Inspectable<CartridgeInfo> {

    Descriptions descriptions = {{

        .name           = "Cartridge",
        .description    = "Cartridge"
    }};

public:

    virtual const CartridgeTraits &getCartridgeTraits() const {

        static CartridgeTraits traits = {

            .type       = CRT_NORMAL,
            .title      = "Cartridge",
        };

        return traits;
    }

    // Maximum number of chip packets on a single cartridge
    static const isize MAX_PACKETS = 128;


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

    isize numPackets = 0;
    CartridgeRom *packet[MAX_PACKETS] = {};

    // Indicates which packets are currently mapped to ROML and ROMH
    isize chipL = 0;
    isize chipH = 0;

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

    // Additional RAM
    u8 *externalRam = nullptr;

    // RAM capacity in bytes
    isize ramCapacity = 0;

    // Total number of write accesses
    i64 writes = 0;


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
    isize switchPos = 0;

    // Status of the cartridge LED (true = on)
    bool led = false;


    //
    // Class methods
    //

public:

    // Checks whether this cartridge has a known type indentifier
    static bool isKnownType(CartridgeType type);

    // Checks whether this cartridge is a supported by the emulator
    static bool isSupportedType(CartridgeType type);

    // Returns true if addr is located in the ROML or the ROMH address space
    static bool isROMLaddr(u16 addr);
    static bool isROMHaddr(u16 addr);

    // Factory methods
    static Cartridge *makeWithType(C64 &c64, CartridgeType type) throws;
    static Cartridge *makeWithCRTFile(C64 &c64, const CRTFile &file) throws;


    //
    // Methods
    //

public:

    Cartridge(C64 &ref);
    ~Cartridge();

    virtual void init();
    void dealloc();

public:

    Cartridge& operator= (const Cartridge& other) {

        cloneRomAndRam(other);

        CLONE(chipL)
        CLONE(chipH)
        CLONE(mappedBytesL)
        CLONE(mappedBytesH)
        CLONE(offsetL)
        CLONE(offsetH)
        CLONE(led)

        CLONE(gameLineInCrtFile)
        CLONE(exromLineInCrtFile)
        CLONE(numPackets)
        CLONE(ramCapacity)
        CLONE(writes)
        CLONE(control)
        CLONE(switchPos)

        return *this;
    }
    void cloneRomAndRam(const Cartridge& other);
    virtual void clone(const Cartridge &other) { *this = other; }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        worker

        << chipL
        << chipH
        << mappedBytesL
        << mappedBytesH
        << offsetL
        << offsetH
        << led;

        if (isResetter(worker)) return;

        worker

        << gameLineInCrtFile
        << exromLineInCrtFile
        << numPackets
        << ramCapacity
        << writes
        << control
        << switchPos;
    }

public:

    void operator << (SerResetter &worker) override;
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override;
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override;


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

protected:

    const char *objectName() const override { return getCartridgeTraits().title; }
    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from Inspectable
    //

public:

    void cacheInfo(CartridgeInfo &result) const override;


    //
    // Analyzing
    //

public:

    // CartridgeInfo getInfo() const;
    CartridgeRomInfo getRomInfo(isize nr) const;

    // Returns the cartridge type
    virtual CartridgeType getCartridgeType() const { return getCartridgeTraits().type; }

    // Checks whether this cartridge is supported by the emulator yet
    bool isSupported() const { return isSupportedType(getCartridgeType()); }


    //
    // Accessing
    //

public:

    // Returns the initial value of the Game or the Exrom line
    virtual bool getGameLineInCrtFile() const { return gameLineInCrtFile; }
    virtual bool getExromLineInCrtFile() const { return exromLineInCrtFile; }

    /* Resets the Game and the Exrom line. The default implementation resets
     * the values to ones found in the CRT file. A few custom cartridges need
     * other start configurations and overwrite this function.
     */
    virtual void resetCartConfig();


    //
    // Handling ROM packets
    //

public:

    // Reads in a chip packet from a CRT file
    virtual void loadChip(isize nr, const CRTFile &c);

    // Banks in a rom chip into the ROML or the ROMH space
    void bankInROML(isize nr, u16 size, u16 offset);
    void bankInROMH(isize nr, u16 size, u16 offset);

    /* Banks in a rom chip. This function calls bankInROML or bankInROMH with
     * the default parameters for this chip as provided in the CRT file.
     */
    virtual void bankIn(isize nr);

    //  Banks out a chip (RAM will be visible again)
    void bankOut(isize nr);


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

    virtual u8 spypeek(u16 addr) const;
    virtual u8 spypeekRomL(u16 addr) const;
    virtual u8 spypeekRomH(u16 addr) const;

    virtual void poke(u16 addr, u8 value);
    virtual void pokeRomL(u16 addr, u8 value) { return; }
    virtual void pokeRomH(u16 addr, u8 value) { return; }

    // Fallthroughs for the I/O spaces
    virtual u8 peekIO1(u16 addr) { return 0; }
    virtual u8 peekIO2(u16 addr) { return 0; }

    virtual u8 spypeekIO1(u16 addr) const { return 0; }
    virtual u8 spypeekIO2(u16 addr) const { return 0; }

    virtual void pokeIO1(u16 addr, u8 value) { }
    virtual void pokeIO2(u16 addr, u8 value) { }


    //
    // Managing on-board RAM
    //

    // Returns the size of the on-board RAM in bytes
    isize getRamCapacity() const { return getCartridgeTraits().memory; }

    /* Assigns external RAM to this cartridge. This functions frees any
     * previously assigned RAM and allocates memory of the specified size. The
     * size is stored in variable ramCapacity.
     */
    void setRamCapacity(isize size);

    // Reads or write RAM cells
    u8 peekRAM(u32 addr) const;
    void pokeRAM(u32 addr, u8 value);
    void eraseRAM(u8 value);


    //
    // Operating buttons
    //

    // Returns the number of available cartridge buttons
    virtual isize numButtons() const { return 0; }

    /* Returns a textual description for a button or nullptr, if there is no
     * button with the specified number.
     */
    virtual const char *getButtonTitle(isize nr) const { return ""; }

    // Presses a button (make sure to call releaseButton() afterwards)
    virtual void pressButton(isize nr) { }

    // Releases a button (make sure to call pressButton() before)
    virtual void releaseButton(isize nr) { }


    //
    // Operating switches
    //

    // Returns the current switch position
    virtual isize getSwitch() const { return switchPos; }
    bool switchIsNeutral() const { return getSwitch() == 0; }
    bool switchIsLeft() const { return getSwitch() < 0; }
    bool switchIsRight() const { return getSwitch() > 0; }

    /* Returns a textual description for a switch position or nullptr if the
     * switch cannot be positioned this way.
     */
    virtual const char *getSwitchDescription(isize pos) const { return ""; }
    const char *getSwitchDescription() const { return getSwitchDescription(getSwitch()); }
    bool validSwitchPosition(isize pos) const { return getSwitchDescription(pos) != nullptr; }

    // Puts the switch in a certain position
    virtual void setSwitch(isize pos);


    //
    // Operating LEDs
    //

    // Returns true if the cartridge has a LED
    // bool hasLED() const { return traits.led; }

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

}
