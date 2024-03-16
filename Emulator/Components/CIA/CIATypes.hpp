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
/// @file

#pragma once

#include "Aliases.h"
#include "Reflection.h"
#include "TODTypes.hpp"

#include "CIATypes.h"

struct CIARevisionEnum : util::Reflection<CIARevisionEnum, CIARevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = MOS_8521;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return nullptr; }
    static const char *key(CIARevision value)
    {
        switch (value) {

            case MOS_6526:      return "MOS_6526";
            case MOS_8521:      return "MOS_8521";
        }
        return "";
    }
};

struct CIARegEnum : util::Reflection<CIARegEnum, CIAReg> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CIAREG_CRB;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CIAREG"; }
    static const char *key(CIAReg value)
    {
        switch (value) {

            case CIAREG_PRA:     return "PRA";
            case CIAREG_PRB:     return "PRB";
            case CIAREG_DDRA:    return "DDRA";
            case CIAREG_DDRB:    return "DDRB";
            case CIAREG_TALO:    return "TALO";
            case CIAREG_TAHI:    return "TAHI";
            case CIAREG_TBLO:    return "TBLO";
            case CIAREG_TBHI:    return "TBHI";
            case CIAREG_TODTHS:  return "TODTHS";
            case CIAREG_TODSEC:  return "TODSEC";
            case CIAREG_TODMIN:  return "TODMIN";
            case CIAREG_TODHR:   return "TODHR";
            case CIAREG_SDR:     return "SDR";
            case CIAREG_ICR:     return "ICR";
            case CIAREG_CRA:     return "CRA";
            case CIAREG_CRB:     return "CRB";
        }
        return "???";
    }
};
