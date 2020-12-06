// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Utils.h"

const char *ciaRegName(u32 addr)
{
    assert(addr < 16);

    static const char *name[16] = {
        
        "PRA",      "PRB",      "DDRA",     "DDRB",
        "TALO",     "TAHI",     "TBLO",     "TBHI",
        "TODLO",    "TODMID",   "TODHI",    "UNUSED",
        "SDR",      "ICR",      "CRA",      "CRB"
    };

    return name[addr];
}
