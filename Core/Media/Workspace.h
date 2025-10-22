// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyFile.h"

namespace vc64 {

class Workspace : public AnyFile {

public:

    static bool isCompatible(const fs::path &path);
    static bool isCompatible(const u8 *buf, isize len) { return false; }
    static bool isCompatible(const Buffer<u8> &buffer) { return false; }
    static bool isCompatible(std::istream &stream) { return false; }


    //
    // Initializing
    //

    Workspace(const fs::path &path) throws { init(path); }

private:

    void init(const fs::path &path) throws;


    //
    // Methods from CoreObject
    //

public:

    const char *objectName() const override { return "Workspace"; }


    //
    // Methods from AnyFile
    //

    FileType type() const override { return FileType::WORKSPACE; }
    bool isCompatiblePath(const fs::path &path) const override { return isCompatible(path); }
    bool isCompatibleBuffer(const u8 *buf, isize len) const override { return isCompatible(buf, len); }
    u64 fnv64() const override { return 0; }
};

}
