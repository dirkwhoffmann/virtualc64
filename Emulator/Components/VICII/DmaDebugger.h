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

class DmaDebugger final : public SubComponent {

    friend class VICII;
    
    Descriptions descriptions = {{

        .name           = "DmaDebugger",
        .description    = "Bus Monitor",
        .shell          = "dmadebugger"
    }};

    Options options = {

        OPT_DMA_DEBUG_ENABLE,
        OPT_DMA_DEBUG_MODE,
        OPT_DMA_DEBUG_OPACITY,
        OPT_VICII_CUT_LAYERS,
        OPT_VICII_CUT_OPACITY,
        OPT_DMA_DEBUG_CHANNEL0,
        OPT_DMA_DEBUG_CHANNEL1,
        OPT_DMA_DEBUG_CHANNEL2,
        OPT_DMA_DEBUG_CHANNEL3,
        OPT_DMA_DEBUG_CHANNEL4,
        OPT_DMA_DEBUG_CHANNEL5,
        OPT_DMA_DEBUG_COLOR0,
        OPT_DMA_DEBUG_COLOR1,
        OPT_DMA_DEBUG_COLOR2,
        OPT_DMA_DEBUG_COLOR3,
        OPT_DMA_DEBUG_COLOR4,
        OPT_DMA_DEBUG_COLOR5
    };

    // Current configuration
    DmaDebuggerConfig config = { };
    
    // Color lookup table. There are 6 colors with 4 different shades
    u32 debugColor[6][4];

    
    //
    // Methods
    //
    
public:
    
    DmaDebugger(C64 &ref);

    DmaDebugger& operator= (const DmaDebugger& other) {

        for (isize i = 0; i < 6; i++) CLONE_ARRAY(debugColor[i])
        CLONE(config)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

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
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from Configurable
    //

public:

    const DmaDebuggerConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


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
