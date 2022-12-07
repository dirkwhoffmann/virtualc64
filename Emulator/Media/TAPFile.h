// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
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

    static bool isCompatible(const string &name);
    static bool isCompatible(std::istream &stream);
    
    
    //
    // Initializing
    //
    
    TAPFile(const string &path) throws { init(path); }
    TAPFile(const u8 *buf, isize len) throws { init(buf, len); }

    
    //
    // Methods from C64Object
    //
    
    const char *getDescription() const override { return "TAPFile"; }
    
    
    //
    // Methods from AnyFile
    //
    
    bool isCompatiblePath(const string &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    FileType type() const override { return FILETYPE_TAP; }
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
