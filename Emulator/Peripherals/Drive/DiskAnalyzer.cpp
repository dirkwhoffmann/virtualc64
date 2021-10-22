// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DiskAnalyzer.h"
#include "Disk.h"

DiskAnalyzer::DiskAnalyzer(Disk *disk) : disk(disk)
{
    msg("DiskAnalyzer::DiskAnalyzer\n");
}

DiskAnalyzer::~DiskAnalyzer()
{
    msg("DiskAnalyzer::~DiskAnalyzer\n");
}
