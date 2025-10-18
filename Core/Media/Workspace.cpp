// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Workspace.h"
#include "IOUtils.h"

#include <sstream>

namespace vc64 {

bool
Workspace::isCompatible(const fs::path &path)
{
    if (!util::isDirectory(path)) return false;

    auto suffix = util::uppercased(path.extension().string());
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
