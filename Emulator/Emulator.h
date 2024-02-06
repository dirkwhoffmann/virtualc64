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

#include "Thread.h"
#include "C64.h"

namespace vc64 {

class Emulator : public CoreComponent { // } : public Thread {

    //
    // The virtual C64
    //

public:

    C64 c64 = C64(*this);


    //
    // Emulator thread
    //

private:


    //
    // Initializing
    //

public:

    Emulator();
    ~Emulator();


    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override { return "Emulator"; }
    void _dump(Category category, std::ostream& os) const override { }


    //
    // Methods from CoreComponent
    //

private:

    void _reset(bool hard) override { };
    isize _size() override { return 0; }
    u64 _checksum() override { return 0; }
    isize _load(const u8 *buffer) override { return 0; }
    isize _save(u8 *buffer) override { return 0; }



    virtual bool isPoweredOff() const override;
    virtual bool isPoweredOn() const override;
    virtual bool isPaused() const override;
    virtual bool isRunning() const override;
    virtual bool isSuspended() const override;
    virtual bool isHalted() const override;

    virtual void suspend() override;
    virtual void resume() override;

};

}
