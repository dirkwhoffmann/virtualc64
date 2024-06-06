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

#include "FileTypes.h"
#include <sstream>
#include <fstream>

namespace vc64 {

class MediaFile {

public:

    virtual ~MediaFile() = default;


    //
    // Static methods
    //

    // Determines the type of an arbitrary file on file
    static FileType type(const string &path);


    //
    // Methods
    //

    // Returns the media type of this file
    virtual FileType type() const { return FILETYPE_UNKNOWN; }

    // Returns a fingerprint (hash value) for this file
    virtual u64 fnv() const = 0;

};

}
