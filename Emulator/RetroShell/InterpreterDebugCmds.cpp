// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "RetroShell.h"
#include "C64.h"
#include "IOUtils.h"
#include "Parser.h"

#include <fstream>
#include <sstream>

namespace vc64 {

void
Interpreter::initDebugShell(Command &root)
{
    initCommons(root);

    //
    // Top-level commands
    //

    root.newGroup("Controlling the instruction stream");

}

}
