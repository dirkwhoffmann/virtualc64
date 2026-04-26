// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/Amiga/FSTypes.h"
#include "utl/abilities/Loggable.h"

namespace retro::vault::amiga {

class FileSystem;

class FSService : public Loggable {

public:

    FileSystem &fs;
    const FSTraits &traits;

    explicit FSService(FileSystem& fs);
};

}
