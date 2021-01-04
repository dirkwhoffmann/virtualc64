// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "CPUPublicTypes.h"
#include "Reflection.h"

//
// Constants
//

#define C_FLAG 0x01
#define Z_FLAG 0x02
#define I_FLAG 0x04
#define D_FLAG 0x08
#define B_FLAG 0x10
#define V_FLAG 0x40
#define N_FLAG 0x80

//
// Reflection APIs
//

struct CPURevisionEnum : Reflection<CPURevisionEnum, CPURevision> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < CPUREV_COUNT;
    }
    
    static const char *prefix() { return nullptr; }
    static const char *key(CPURevision value)
    {
        switch (value) {
                
            case MOS_6510:      return "MOS_6510";
            case MOS_6502:      return "MOS_6502";
            case CPUREV_COUNT:  return "???";
        }
        return "???";
    }
};

struct IntSourceEnum : Reflection<IntSourceEnum, IntSource> {
    
    static bool isValid(long value)
    {
        return
        value == INTSRC_CIA  ||
        value == INTSRC_VIC  ||
        value == INTSRC_VIA1 ||
        value == INTSRC_VIA2 ||
        value == INTSRC_EXP  ||
        value == INTSRC_KBD;
    }
    
    static const char *prefix() { return "INTSRC"; }
    static const char *key(IntSource value)
    {
        switch (value) {
                
            case INTSRC_CIA:   return "CIA";
            case INTSRC_VIC:   return "VIC";
            case INTSRC_VIA1:  return "VIA1";
            case INTSRC_VIA2:  return "VIA2";
            case INTSRC_EXP:   return "EXP";
            case INTSRC_KBD:   return "KBD";
        }
        return "???";
    }
    
    static bool verify(long nr) { return Reflection::verify(nr, INTSRC_KBD); }
};

struct BreakpointTypeEnum : Reflection<BreakpointTypeEnum, BreakpointType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < BPTYPE_COUNT;
    }
    
    static const char *prefix() { return "BPTYPE"; }
    static const char *key(BreakpointType value)
    {
        switch (value) {
                
            case BPTYPE_NONE:   return "NONE";
            case BPTYPE_HARD:   return "HARD";
            case BPTYPE_SOFT:   return "SOFT";
            case BPTYPE_COUNT:  return "???";
        }
        return "???";
    }
};

//
// Private types
//

typedef enum
{
    ADDR_IMPLIED,
    ADDR_ACCUMULATOR,
    ADDR_IMMEDIATE,
    ADDR_ZERO_PAGE,
    ADDR_ZERO_PAGE_X,
    ADDR_ZERO_PAGE_Y,
    ADDR_ABSOLUTE,
    ADDR_ABSOLUTE_X,
    ADDR_ABSOLUTE_Y,
    ADDR_INDIRECT_X,
    ADDR_INDIRECT_Y,
    ADDR_RELATIVE,
    ADDR_DIRECT,
    ADDR_INDIRECT
}
AddressingMode;
