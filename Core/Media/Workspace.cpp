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

#include "config.h"
#include "Workspace.h"
#include "utl/io/Files.h"
#include "utl/support/Strings.h"

#include <sstream>

namespace vc64 {

bool
Workspace::isCompatible(const fs::path &path)
{
    if (!utl::isDirectory(path)) return false;

    auto suffix = utl::uppercased(path.extension().string());
    return suffix == ".VC64";
}

void
Workspace::init(const fs::path &path)
{
    // Only proceed if the provided filename points to a directory
    if (!isCompatiblePath(path)) throw AppError(Fault::FILE_TYPE_MISMATCH);

    this->path = path;
}

}
