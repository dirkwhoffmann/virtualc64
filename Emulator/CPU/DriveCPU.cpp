// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

#include "DriveCPU.h"

DriveCPU::DriveCPU(CPUModel model, C64& ref, Memory &memref) : CPU(model, ref, memref)
{
}
