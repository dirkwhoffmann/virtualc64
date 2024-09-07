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

#include "ExpansionPortTypes.h"
#include "CmdQueueTypes.h"
#include "SubComponent.h"
#include "Cartridge.h"

namespace vc64 {

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

class ExpansionPort final : public SubComponent, public Inspectable<CartridgeInfo> {
    
    Descriptions descriptions = {{

        .type           = ExpansionPortClass,
        .name           = "Expansion",
        .description    = "Expansion Port",
        .shell          = "expansion"
    }};

    Options options = {

        OPT_EXP_REU_SPEED
    };
    
    // Current configuration
    ExpansionPortConfig config = { };

    // Attached cartridge or nullptr
    std::unique_ptr<Cartridge> cartridge;
    
    // Type of the attached cartridge
    CartridgeType crtType = CRT_NONE;
    
    // Values of the Game and the Exrom line (true if no cartridge is attached)
    bool gameLine = 1;
    bool exromLine = 1;

    
    //
    // Methods
    //
    
public:
    
    ExpansionPort(C64 &ref) : SubComponent(ref) { };
    ExpansionPort& operator= (const ExpansionPort& other);


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << crtType
        << gameLine
        << exromLine

        << config.reuSpeed;
    }
    
    void operator << (SerResetter &worker) override;
    void operator << (SerChecker &worker) override;
    void operator << (SerCounter &worker) override;
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override;


    //
    // Methods from CoreComponent
    //

public:
    
    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;
    void _didReset(bool hard) override;


    //
    // Methods from Configurable
    //

public:

    const ExpansionPortConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;

    //
    // Analyzing
    //
    
public:

    const CartridgeTraits &getCartridgeTraits() const;
    const CartridgeInfo &getInfo() const;
    CartridgeRomInfo getRomInfo(isize nr) const;
    CartridgeType getCartridgeType() const;

    bool hasReu() const { return getCartridgeType() == CRT_REU; }

    
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

    void resetCartConfig();


    //
    // Attaching and detaching
    //

    // Attaches a cartridge to the expansion port
    void attachCartridge(const fs::path &path, bool reset = true) throws;
    void attachCartridge(const MediaFile &file, bool reset = true) throws;
    void attachCartridge(Cartridge *c);
    void attachReu(isize capacity);
    void attachGeoRam(isize capacity);
    void attachIsepicCartridge();

    // Removes a cartridge from the expansion port (if any)
    void detachCartridge();


    //
    // Operating buttons, switches, and LEDs
    //

    // Presses a button (make sure to call releaseButton() afterwards)
    void pressButton(isize nr);
    
    // Releases a button (make sure to call pressButton() before)
    void releaseButton(isize nr);

    // Puts the switch in the provided position
    void setSwitch(isize pos) { if (cartridge) cartridge->setSwitch(pos); }
    
    // Switches the LED on or off
    void setLED(bool value);
    

    //
    // Processing commands and events
    //

public:

    // Processes a cartridge command
    void processCommand(const Cmd &cmd);

    // Services an event in the EXP slot
    void processEvent(EventID id);


    //
    // Cycle-accurate execution
    //

public:

    /* Indicates whether a cartridge is attached that needs cycle-accurate
     * emulation. If such a cartridge is attached, the main execution routine
     * calls execute() function in every cycle.
     */
    bool needsAccurateEmulation();

    // Passes control to the cartridge in every cycle.
    void execute();


    //
    // Handling delegation calls
    //
    
    /* This function is invoked by the expansion port at the end of each frame.
     * Only a few cartridges such as EpyxFastLoader will do some action here.
     */
    void endOfFrame();
    
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

}
