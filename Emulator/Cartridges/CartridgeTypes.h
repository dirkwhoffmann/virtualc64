// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "CartridgePublicTypes.h"
#include "Reflection.h"

//
// Reflection APIs
//

struct FlashStateEnum : util::Reflection<FlashStateEnum, FlashState> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < FLASH_COUNT;
    }
    
    static const char *prefix() { return "FLASH"; }
    static const char *key(FlashState value)
    {
        switch (value) {
                
            case FLASH_READ:                 return "READ";
            case FLASH_MAGIC_1:              return "MAGIC_1";
            case FLASH_MAGIC_2:              return "MAGIC_2";
            case FLASH_AUTOSELECT:           return "AUTOSELECT";
            case FLASH_BYTE_PROGRAM:         return "BYTE_PROGRAM";
            case FLASH_BYTE_PROGRAM_ERROR:   return "BYTE_PROGRAM_ERROR";
            case FLASH_ERASE_MAGIC_1:        return "ERASE_MAGIC_1";
            case FLASH_ERASE_MAGIC_2:        return "ERASE_MAGIC_2";
            case FLASH_ERASE_SELECT:         return "ERASE_SELECT";
            case FLASH_CHIP_ERASE:           return "CHIP_ERASE";
            case FLASH_SECTOR_ERASE:         return "SECTOR_ERASE";
            case FLASH_SECTOR_ERASE_TIMEOUT: return "SECTOR_ERASE_TIMEOUT";
            case FLASH_SECTOR_ERASE_SUSPEND: return "SECTOR_ERASE_SUSPEND";
            case FLASH_COUNT:                return "???";
        }
        return "???";
    }
};
