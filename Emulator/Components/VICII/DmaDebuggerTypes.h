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
#include "BusTypes.h"

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
