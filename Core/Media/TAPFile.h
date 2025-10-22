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

#include "AnyFile.h"

namespace vc64 {

class TAPFile : public AnyFile {

    // File pointer (used by read() and seek())
    isize fp = -1;
    
public:

    //
    // Class methods
    //

    static bool isCompatible(const fs::path &path);
    static bool isCompatible(const u8 *buf, isize len);
    static bool isCompatible(const Buffer<u8> &buffer);


    //
    // Initializing
    //
    
    TAPFile(const fs::path &path) throws { init(path); }
    TAPFile(const u8 *buf, isize len) throws { init(buf, len); }

    
    //
    // Methods from CoreObject
    //
    
    const char *objectName() const override { return "TAPFile"; }
    
    
    //
    // Methods from AnyFile
    //
    
    bool isCompatiblePath(const fs::path &path) const override { return isCompatible(path); }
    bool isCompatibleBuffer(const u8 *buf, isize len) const override { return isCompatible(buf, len); }
    FileType type() const override { return FileType::TAP; }
    PETName<16> getName() const override;
    void finalizeRead() override;

    
    //
    // Reading
    //
    
    // Returns the TAP version (0 = standard layout, 1 = extended layout)
    TAPVersion version() const { return (TAPVersion)data[0x000C]; }
    
    // Returns the position of the first pulse byte
    isize headerSize() const;
    
    // Returns the number of stored pulses
    isize numPulses();

    // Sets the file pointer to a specific pulse
    void seek(isize nr);
    
    // Reads the next pulse and advances the file pointer
    isize read();
};

}
