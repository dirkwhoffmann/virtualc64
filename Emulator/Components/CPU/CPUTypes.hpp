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

#include "Aliases.h"
#include "Reflection.hpp"

#include "CPUTypes.h"

struct CPURevisionEnum : util::Reflection<CPURevisionEnum, vc64::peddle::CPURevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = vc64::peddle::MOS_8502;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return nullptr; }
    static const char *key(vc64::peddle::CPURevision value)
    {
        switch (value) {

            case vc64::peddle::MOS_6502:  return "MOS_6502";
            case vc64::peddle::MOS_6507:  return "MOS_6507";
            case vc64::peddle::MOS_6510:  return "MOS_6510";
            case vc64::peddle::MOS_8502:  return "MOS_8502";
        }
        return "???";
    }
};
