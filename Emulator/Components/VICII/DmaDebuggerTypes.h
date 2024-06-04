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

#include "Reflection.h"
#include "BusTypes.h"

namespace vc64 {

//
// Enumerations
//

/// DMA debugger display mode
enum_long(DMA_DISPLAY_MODE)
{
    DMA_DISPLAY_MODE_FG_LAYER,          ///< Modulate the foreground layer
    DMA_DISPLAY_MODE_BG_LAYER,          ///< Modulate the background layer
    DMA_DISPLAY_MODE_ODD_EVEN_LAYERS    ///< Modulate both layers
};
typedef DMA_DISPLAY_MODE DmaDisplayMode;

struct DmaDisplayModeEnum : util::Reflection<DmaDisplayModeEnum, DmaDisplayMode> {
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = DMA_DISPLAY_MODE_ODD_EVEN_LAYERS;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "DMA_DISPLAY_MODE"; }
    static const char *key(long value)
    {
        switch (value) {
                
            case DMA_DISPLAY_MODE_FG_LAYER:         return "FG_LAYER";
            case DMA_DISPLAY_MODE_BG_LAYER:         return "BG_LAYER";
            case DMA_DISPLAY_MODE_ODD_EVEN_LAYERS:  return "ODD_EVEN_LAYERS";
        }
        return "???";
    }
};


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

}
