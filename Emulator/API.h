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

/* 
 * This files declares the emulator's public API.
 */

//
// Emulator
//

// Launches the emulator thread
void launch(const void *listener, Callback *func);

// Configures the emulator to match a specific C64 model
void configure(C64Model model);

// Sets a single configuration option
void configure(Option option, i64 value) throws;
void configure(Option option, long id, i64 value) throws;

// Queries a single configuration option
i64 getConfigItem(Option option) const;
i64 getConfigItem(Option option, long id) const;
void setConfigItem(Option option, i64 value);

// Returns the emulated refresh rate of the virtual C64
double refreshRate() const override;


//
// C64
//

struct C64_API : API {

    using API::API;

    // Performs a hard or soft reset
    void hardReset();
    void softReset();

} c64;


//
// CIAs
//

struct CIA_API : API {

    CIA &cia;
    CIA_API(Emulator &emu, CIA& cia) : API(emu), cia(cia) { }

    CIAInfo getInfo() const;

} cia1, cia2;


