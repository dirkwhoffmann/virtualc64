// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef C64_PRIVATE_TYPES_H
#define C64_PRIVATE_TYPES_H

#include "VICIIPrivateTypes.h"
#include "DiskPrivateTypes.h"
#include "CIAPrivate.h"

enum_int(ACTION_FLAG)
{
    ACTION_FLAG_STOP          = 0b0000001,
    ACTION_FLAG_CPU_JAMMED    = 0b0000010,
    ACTION_FLAG_INSPECT       = 0b0000100,
    ACTION_FLAG_BREAKPOINT    = 0b0001000,
    ACTION_FLAG_WATCHPOINT    = 0b0010000,
    ACTION_FLAG_AUTO_SNAPSHOT = 0b0100000,
    ACTION_FLAG_USER_SNAPSHOT = 0b1000000
};
typedef ACTION_FLAG ActionFlag;

#endif
