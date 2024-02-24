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

#include "DmaDebuggerTypes.h"
#include "SubComponent.h"
#include "Colors.h"

namespace vc64 {

class DmaDebugger final : public SubComponent, public Dumpable {

    friend class VICII;
    
    ConfigOptions options = {

        { OPT_DMA_DEBUG_ENABLE,     "Enable or disable DMA debugging" },
        { OPT_DMA_DEBUG_MODE,       "Overlay mode" },
        { OPT_DMA_DEBUG_OPACITY,    "Overlay opacity" },
        { OPT_VICII_CUT_LAYERS,     "Cutout graphics layers" },
        { OPT_VICII_CUT_OPACITY,    "Cutout opacity" },
        { OPT_DMA_DEBUG_CHANNEL0,   "Enable or disable channel 0" },
        { OPT_DMA_DEBUG_CHANNEL1,   "Enable or disable channel 1" },
        { OPT_DMA_DEBUG_CHANNEL2,   "Enable or disable channel 2" },
        { OPT_DMA_DEBUG_CHANNEL3,   "Enable or disable channel 3" },
        { OPT_DMA_DEBUG_CHANNEL4,   "Enable or disable channel 4" },
        { OPT_DMA_DEBUG_CHANNEL5,   "Enable or disable channel 5" },
        { OPT_DMA_DEBUG_COLOR0,     "Color for channel 0" },
        { OPT_DMA_DEBUG_COLOR1,     "Color for channel 1" },
        { OPT_DMA_DEBUG_COLOR2,     "Color for channel 2" },
        { OPT_DMA_DEBUG_COLOR3,     "Color for channel 3" },
        { OPT_DMA_DEBUG_COLOR4,     "Color for channel 4" },
        { OPT_DMA_DEBUG_COLOR5,     "Color for channel 5" }
    };

    // Current configuration
    DmaDebuggerConfig config = { };
    
    // Color lookup table. There are 6 colors with different shades
    // TODO: MOVE TO CONFIG
    u32 debugColor[6][4];

    
    //
    // Initializing
    //
    
public:
    
    DmaDebugger(C64 &ref);
    
    
    //
    // Methods from CoreObject
    //

    const char *getDescription() const override { return "DmaDebugger"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //

public:
       
    DmaDebugger& operator= (const DmaDebugger& other) {

        for (isize i = 0; i < 6; i++) CLONE_ARRAY(debugColor[i])
        CLONE(config)

        return *this;
    }

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << debugColor

        << config.dmaDebug
        << config.dmaChannel
        << config.dmaColor
        << config.dmaDisplayMode
        << config.dmaOpacity;

    } SERIALIZERS(serialize);


    //
    // Methods from Configurable
    //

    const ConfigOptions &getOptions() const override { return options; }

    // Gets or sets a config option
    i64 getOption(Option opt) const override;
    void setOption(Option opt, i64 value) override;


    //
    // Configuring
    //
    
public:
    
    // static DmaDebuggerConfig getDefaultConfig();
    const DmaDebuggerConfig &getConfig() const { return config; }
    void resetConfig() override;

    
    //
    // Managing colors
    //
    
    void setDmaDebugColor(MemAccess type, GpuColor color);
    void setDmaDebugColor(MemAccess type, RgbColor color);

    
    //
    // Visualizing DMA
    //
    
public:
    
    // Visualizes a memory access by drawing into the DMA debuger texture
    void visualizeDma(isize offset, u8 data, MemAccess type);
    void visualizeDma(u32 *ptr, u8 data, MemAccess type);
    
    // Superimposes the debug output onto the current scanline
    void computeOverlay(u32 *emuTexture, u32 *dmaTexture);

    
    //
    // Cutting layers
    //
    
public:
    
    // Cuts out certain graphics layers
    void cutLayers();
};

}
