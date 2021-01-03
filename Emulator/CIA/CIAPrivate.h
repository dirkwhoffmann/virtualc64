// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CIA_PRIVATE_H
#define _CIA_PRIVATE_H

#include "Reflection.h"
#include "CIAPublic.h"

struct CIARevisionEnum : Reflection<CIARevisionEnum, CIARevision> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < CIAREV_COUNT;
    }

    static const char *prefix() { return nullptr; }
    static const char *key(CIARevision value)
    {
        switch (value) {
                
            case MOS_6526:      return "MOS_6526";
            case MOS_8521:      return "MOS_8521";
            case CIAREV_COUNT:  return "???";
        }
        return "";
    }
};

struct CIARegEnum : Reflection<CIARegEnum, CIAReg> {

    static bool isValid(long value)
    {
        return (unsigned long)value < CIAREG_COUNT;
    }
    
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
            case CIAREG_COUNT:   return "???";
        }
        return "???";
    }
};

#endif
