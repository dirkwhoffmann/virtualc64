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

#include "ParCableTypes.h"
#include "DriveTypes.h"
#include "SubComponent.h"

namespace vc64 {

class ParCable : public SubComponent, public Dumpable {
    
    //
    // Initializing
    //
    
public:
    
    ParCable(C64 &ref);
    
    
    //
    // Methods from CoreObject
    //

private:
    
    const char *getDescription() const override { return "ParCable"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //

public:

    ParCable& operator= (const ParCable& other) { return *this; }
    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);

    
    //
    // Using
    //
    
public:
    
    // Returns the current value on the cable
    u8 getValue() const;

    // Sends a handshake signal
    void driveHandshake();
    void c64Handshake();
    
private:
    
    void c64Handshake(Drive &drive);
    
private:
    
    // Collects port values
    u8 getCIA() const;
    u8 getVIA(const Drive &drive) const;
    u8 getPIA(const Drive &drive) const;
};

}
