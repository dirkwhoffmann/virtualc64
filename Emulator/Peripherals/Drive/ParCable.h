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

class ParCable final : public SubComponent {
        
    Descriptions descriptions = {{

        .name           = "ParCable",
        .description    = "Parallel Drive Cable"
    }};

public:
    
    ParCable(C64 &ref);

    ParCable& operator= (const ParCable& other) { return *this; }


    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);

    
    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;


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
    u8 getCIA(u8 cable) const;
    u8 getVIA(const Drive &drive) const;
    u8 getPIA(const Drive &drive) const;
};

}
