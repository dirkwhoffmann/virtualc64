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

#include "BusTypes.h"

namespace vc64 {

//
// Enumerations
//

/// X-Ray mode
enum class XRayMode : long
{
    XRAY_NONE,
    XRAY_DMA,
    XRAY_LAYERS
};

struct XRayModeEnum : Reflectable<XRayModeEnum, XRayMode> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(XRayMode::XRAY_LAYERS);

    static const char *_key(XRayMode value)
    {
        switch (value) {

            case XRayMode::XRAY_NONE:   return "XRAY_NONE";
            case XRayMode::XRAY_DMA:    return "XRAY_DMA";
            case XRayMode::XRAY_LAYERS: return "XRAY_LAYERS";
        }
        return "???";
    }
    static const char *help(XRayMode value)
    {
        switch (value) {

            case XRayMode::XRAY_NONE:   return "Off";
            case XRayMode::XRAY_DMA:    return "DMA debugger";
            case XRayMode::XRAY_LAYERS: return "Layer inspector";
        }
        return "???";
    }
};

/// DMA debugger display mode
enum class DmaDisplayMode : long
{
    FG_LAYER,
    BG_LAYER,
    ODD_EVEN_LAYERS
};

struct DmaDisplayModeEnum : Reflectable<DmaDisplayModeEnum, DmaDisplayMode> {
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DmaDisplayMode::ODD_EVEN_LAYERS);
    
    static const char *_key(DmaDisplayMode value)
    {
        switch (value) {
                
            case DmaDisplayMode::FG_LAYER:        return "FG_LAYER";
            case DmaDisplayMode::BG_LAYER:        return "BG_LAYER";
            case DmaDisplayMode::ODD_EVEN_LAYERS: return "ODD_EVEN_LAYERS";
        }
        return "???";
    }
    static const char *help(DmaDisplayMode value)
    {
        switch (value) {
                
            case DmaDisplayMode::FG_LAYER:        return "Foreground layer";
            case DmaDisplayMode::BG_LAYER:        return "Background layer";
            case DmaDisplayMode::ODD_EVEN_LAYERS: return "Mixed layers";
        }
        return "???";
    }
};


//
// Structures
//

typedef struct
{
    // X-Ray mode (off, DMA debugger, layer inspector)
    XRayMode mode;
    bool dmaOverlay;
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
