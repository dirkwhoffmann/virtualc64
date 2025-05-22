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

#include "MemoryTypes.h"
#include "SubComponent.h"

namespace vc64 {

class MemoryDebugger final : public SubComponent {

    Descriptions descriptions = {{

        .name           = "MemoryDebugger",
        .description    = "Memory Debugger",
        .shell          = ""
    }};

    Options options = {

    };
    
public:

    // Last used address (current object location)
    u16 current = 0;


    //
    // Methods
    //

public:

    using SubComponent::SubComponent;

    MemoryDebugger& operator= (const Host& other) {

        return *this;
    }


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
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }
    

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

    // Loads a chunk of RAM from a stream or file
    void load(std::istream& is, u16 addr);
    void load(fs::path& path, u16 addr);

    // Saves a chunk of RAM to a stream or file
    void save(std::ostream& is, u16 addr, isize count);
    void save(fs::path& path, u16 addr, isize count);

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
