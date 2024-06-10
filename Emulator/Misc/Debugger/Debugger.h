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

#include "DebuggerTypes.h"
#include "SubComponent.h"

namespace vc64 {

class Debugger : public SubComponent {

    Descriptions descriptions = {{

        .name           = "Debugger",
        .description    = "Debugger"
    }};

public:

    // Last used address (current object location)
    u16 current = 0;


    //
    // Methods
    //

public:

    using SubComponent::SubComponent;


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


    //
    // Managing memory
    //

public:

    // Returns a memory dump in ASCII, hex, or both
    isize dump(char *dst, u16 addr, const char *fmt) const;
    isize dump(std::ostream& os, u16 addr, const char *fmt) const;

    // Writes a memory dump into a stream
    isize ascDump(std::ostream& os, u16 addr, isize lines);
    isize hexDump(std::ostream& os, u16 addr, isize lines);
    isize memDump(std::ostream& os, u16 addr, isize lines);

    // Searches a number sequence in memory
    isize memSearch(const string &pattern, u16 addr);

    // Reads a value from memory
    u32 read(u32 addr, isize sz);

    // Writes a value into memory (multiple times)
    void write(u16 addr, u8 val, isize repeats = 1);

    // Copies a chunk of memory
    void copy(u16 src, u16 dst, isize cnt = 1);


    //
    // Displaying expressions
    //

    // Displays a value in different number formats (hex, dec, bin, alpha)
    void convertNumeric(std::ostream& os, u8 value) const;
    void convertNumeric(std::ostream& os, u16 value) const;
    void convertNumeric(std::ostream& os, u32 value) const;
    void convertNumeric(std::ostream& os, string value) const;
};


}
