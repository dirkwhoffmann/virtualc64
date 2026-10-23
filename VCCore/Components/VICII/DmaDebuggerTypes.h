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
/// Number of persisted Opt::XRAY_COLORn slots
constexpr isize XRAY_COLOR_COUNT = 11;

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

/// Who merges the X-Ray texture into the emulator texture
enum class XRayCompositor : long
{
    CPU,    // The emulator does it
    GPU     // The host does it (e.g., in its merge shader)
};

struct XRayCompositorEnum : Reflectable<XRayCompositorEnum, XRayCompositor> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(XRayCompositor::GPU);

    static const char *_key(XRayCompositor value)
    {
        switch (value) {

            case XRayCompositor::CPU:   return "CPU";
            case XRayCompositor::GPU:   return "GPU";
        }
        return "???";
    }
    static const char *help(XRayCompositor value)
    {
        switch (value) {

            case XRayCompositor::CPU:   return "Composed by the emulator";
            case XRayCompositor::GPU:   return "Composed by the host";
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
    XRayCompositor compositor;
    bool dmaChannel[6];
    // Colors of the DMA channels (0 - 5) and of the cut-out layers (0 - 10).
    // The layers use sprite n : color n, border : 8, foreground : 9,
    // background : 10. Hence, the colors of the first channels are shared.
    u32 dmaColor[XRAY_COLOR_COUNT];
    DmaDisplayMode dmaDisplayMode;
    u8 dmaOpacity;
    
    // Layers cut out in XRayMode::XRAY_LAYERS
    // Bits 0 - 7: sprites, 9: background, 10: foreground, 11: border
    u16 cutLayers;
}
DmaDebuggerConfig;

}
