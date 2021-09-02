// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"
#include "BusTypes.h"

//
// Enumerations
//

enum_long(DMA_DISPLAY_MODE)
{
    DMA_DISPLAY_MODE_FG_LAYER,
    DMA_DISPLAY_MODE_BG_LAYER,
    DMA_DISPLAY_MODE_ODD_EVEN_LAYERS
};
typedef DMA_DISPLAY_MODE DmaDisplayMode;

#ifdef __cplusplus
struct DmaDisplayModeEnum : util::Reflection<DmaDisplayModeEnum, DmaDisplayMode> {
    
    static long min() { return 0; }
    static long max() { return DMA_DISPLAY_MODE_ODD_EVEN_LAYERS; }
    static bool isValid(long value) { return value >= min() && value <= max(); }
    
    static const char *prefix() { return "DMA_DISPLAY_MODE"; }
    static const char *key(DmaDisplayMode value)
    {
        switch (value) {
                
            case DMA_DISPLAY_MODE_FG_LAYER:         return "FG_LAYER";
            case DMA_DISPLAY_MODE_BG_LAYER:         return "BG_LAYER";
            case DMA_DISPLAY_MODE_ODD_EVEN_LAYERS:  return "ODD_EVEN_LAYERS";
        }
        return "???";
    }
};
#endif


//
// Structures
//

typedef struct
{
    // DMA debugger
    bool dmaDebug;
    bool dmaChannel[6];
    u32 dmaColor[6];
    DmaDisplayMode dmaDisplayMode;
    u8 dmaOpacity;
    
    // Cutter
    u16 cutLayers;
    u8 cutOpacity;
}
DmaDebuggerConfig;
